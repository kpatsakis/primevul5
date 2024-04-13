int qh_init(const char *path)
{
	int result    = 0;
	int old_umask = 0;

	if (qh_listen_sock >= 0) {
		iobroker_close(nagios_iobs, qh_listen_sock);
	}

	if (path == NULL) {

		logit(NSLOG_RUNTIME_ERROR, TRUE, "qh: query_socket is NULL. What voodoo is this?\n");
		return ERROR;
	}

	old_umask = umask(0117);
	errno = 0;
	qh_listen_sock = nsock_unix(path, NSOCK_TCP | NSOCK_UNLINK);
	umask(old_umask);

	if (qh_listen_sock < 0) {

		logit(NSLOG_RUNTIME_ERROR, TRUE, "qh: Failed to init socket '%s'. %s: %s\n",
			  path, nsock_strerror(qh_listen_sock), strerror(errno));
		return ERROR;
	}

	/* plugins shouldn't have this socket */
	result = fcntl(qh_listen_sock, F_SETFD, FD_CLOEXEC);
	if (result == -1) {

		logit(NSLOG_RUNTIME_ERROR, TRUE, "qh: Failed to fcntl() query handler socket\n");
	}

	/* most likely overkill, but it's small, so... */
	qh_table = dkhash_create(1024);
	if (qh_table == NULL) {

		logit(NSLOG_RUNTIME_ERROR, TRUE, "qh: Failed to create hash table\n");
		close(qh_listen_sock);
		return ERROR;
	}

	errno = 0;
	result = iobroker_register(nagios_iobs, qh_listen_sock, NULL, qh_input);
	if (result < 0) {

		dkhash_destroy(qh_table);
		close(qh_listen_sock);
		logit(NSLOG_RUNTIME_ERROR, TRUE, "qh: Failed to register socket with io broker: %s; errno=%d: %s\n", iobroker_strerror(result), errno, strerror(errno));
		return ERROR;
	}

	logit(NSLOG_INFO_MESSAGE, FALSE, "qh: Socket '%s' successfully initialized\n", path);

	/* now register our the in-core handlers */
	result = qh_register_handler("core", "Nagios Core control and info", 0, qh_core);
	if (result == OK) {
		logit(NSLOG_INFO_MESSAGE, FALSE, "qh: core query handler registered\n");
	}

	result = qh_register_handler("echo", "The Echo Service - What You Put Is What You Get", 0, qh_echo);
	if (result == OK) {
		logit(NSLOG_INFO_MESSAGE, FALSE, "qh: echo service query handler registered\n");
	}

	result = qh_register_handler("help", "Help for the query handler", 0, qh_help);
	if (result == OK) {
		logit(NSLOG_INFO_MESSAGE, FALSE, "qh: help for the query handler registered\n");
	}

	return 0;
}