static int qh_input(int sd, int events, void *ioc_)
{
	iocache * ioc = (iocache *) ioc_;
	int result    = 0;

	/* 
		input on main socket, so accept one 
		this is when a worker initially connects
		we create the iocache and then register
		that to a new socket descriptor and this function
		so that ioc_ != NULL next time
	*/
	if (sd == qh_listen_sock) {

		struct sockaddr sa;
		socklen_t slen = 0;
		int nsd        = 0;

		/* shut valgrind up */
		memset(&sa, 0, sizeof(sa));
		nsd = accept(sd, &sa, &slen);
		if (qh_max_running && qh_running >= qh_max_running) {
			nsock_printf(nsd, "503: Server full");
			close(nsd);
			return 0;
		}

		ioc = iocache_create(16384);
		if (ioc == NULL) {
			logit(NSLOG_RUNTIME_ERROR, TRUE, "qh: Failed to create iocache for inbound request\n");
			nsock_printf(nsd, "500: Internal server error");
			close(nsd);
			return 0;
		}

		/*
		 * @todo: Stash the iocache and the socket in some
		 * addressable list so we can release them on deinit
		 */
		result = iobroker_register(nagios_iobs, nsd, ioc, qh_input);
		if (result < 0) {
			logit(NSLOG_RUNTIME_ERROR, TRUE, "qh: Failed to register input socket %d with I/O broker: %s\n", nsd, strerror(errno));
			iocache_destroy(ioc);
			close(nsd);
			return 0;
		}

		/* make it non-blocking, but leave kernel buffers unchanged */
		worker_set_sockopts(nsd, 0);
		qh_running++;
		return 0;
	}

	/*
		this is when an existing connection
		sends more data after they've already made
		the connection
	*/
	else {

		unsigned long len         = 0;
		unsigned int query_len    = 0;
		struct query_handler * qh = NULL;
		char * buf                = NULL;
		char * space              = NULL;
		char * handler            = NULL;
		char * query              = NULL;

		result = iocache_read(ioc, sd);

		/* disconnect? */
		if (result == 0 || (result < 0 && errno == EPIPE)) {
			iocache_destroy(ioc);
			iobroker_close(nagios_iobs, sd);
			qh_running--;
			return 0;
		}

		/*
		 * A request looks like this: '[@|#]<qh>[<SP>][<query>]\0'.
		 * That is, optional '#' (oneshot) or '@' (keepalive),
		 * followed by the name of a registered handler, followed by
		 * an optional space and an optional query. If the handler
		 * has no "default" handler, a query is required or an error
		 * will be thrown.
		 */

		/* Use data up to the first nul byte */
		buf = iocache_use_delim(ioc, "\0", 1, &len);
		if (buf == NULL) {
			return 0;
		}

		/* Identify handler part and any magic query bytes */
		if (*buf == '@' || *buf == '#') {
			handler = buf + 1;
		}

		/* Locate query (if any) */
		space = strchr(buf, ' ');
		if (space != NULL) {
			*space = 0;
			query = space + 1;
			query_len = len - (unsigned long)(query - buf);
		}

		/* locate the handler */
		qh = qh_find_handler(handler);

		/* not found. that's a 404 */
		if (qh == NULL) {
			nsock_printf(sd, "404: %s: No such handler", handler);
			iobroker_close(nagios_iobs, sd);
			iocache_destroy(ioc);
			return 0;
		}

		/* strip trailing newlines */
		while (query_len > 0
		       && (query[query_len - 1] == 0 || query[query_len - 1] == '\n')) {

			query[--query_len] = 0;	
		}

		/* now pass the query to the handler */
		result = qh->handler(sd, query, query_len);
		if (result >= 100) {
			nsock_printf_nul(sd, "%d: %s", result, qh_strerror(result));
		}

		/* error code or one-shot query */
		if (result >= 300 || *buf == '#') {
			iobroker_close(nagios_iobs, sd);
			iocache_destroy(ioc);
			return 0;
		}

		/* check for magic handler codes */
		switch (result) {

		/* oneshot handler */
		case QH_CLOSE:

		/* general error */
		case -1:
			iobroker_close(nagios_iobs, sd);

		/* fallthrough */

		/* handler takes over */
		case QH_TAKEOVER:

		/* switch protocol (takeover + message) */
		case 101:
			iocache_destroy(ioc);
			break;
		}
	}

	return 0;
}