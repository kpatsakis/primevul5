static int dns_finalize_config(void)
{
	struct dns_resolvers *resolvers;
	struct proxy	     *px;
	int err_code = 0;

	/* allocate pool of resolution per resolvers */
	list_for_each_entry(resolvers, &dns_resolvers, list) {
		struct dns_nameserver *ns;
		struct task           *t;

		/* Check if we can create the socket with nameservers info */
		list_for_each_entry(ns, &resolvers->nameservers, list) {
			struct dgram_conn *dgram = NULL;
			int fd;

			/* Check nameserver info */
			if ((fd = socket(ns->addr.ss_family, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
				ha_alert("config : resolvers '%s': can't create socket for nameserver '%s'.\n",
					 resolvers->id, ns->id);
				err_code |= (ERR_ALERT|ERR_ABORT);
				continue;
			}
			if (connect(fd, (struct sockaddr*)&ns->addr, get_addr_len(&ns->addr)) == -1) {
				ha_alert("config : resolvers '%s': can't connect socket for nameserver '%s'.\n",
					 resolvers->id, ns->id);
				close(fd);
				err_code |= (ERR_ALERT|ERR_ABORT);
				continue;
			}
			close(fd);

			/* Create dgram structure that will hold the UPD socket
			 * and attach it on the current nameserver */
			if ((dgram = calloc(1, sizeof(*dgram))) == NULL) {
				ha_alert("config: resolvers '%s' : out of memory.\n",
					 resolvers->id);
				err_code |= (ERR_ALERT|ERR_ABORT);
				goto err;
			}

			/* Leave dgram partially initialized, no FD attached for
			 * now. */
			dgram->owner     = ns;
			dgram->data      = &resolve_dgram_cb;
			dgram->t.sock.fd = -1;
			ns->dgram        = dgram;
		}

		/* Create the task associated to the resolvers section */
		if ((t = task_new(MAX_THREADS_MASK)) == NULL) {
			ha_alert("config : resolvers '%s' : out of memory.\n", resolvers->id);
			err_code |= (ERR_ALERT|ERR_ABORT);
			goto err;
		}

		/* Update task's parameters */
		t->process   = dns_process_resolvers;
		t->context   = resolvers;
		resolvers->t = t;
		task_wakeup(t, TASK_WOKEN_INIT);
	}

	for (px = proxies_list; px; px = px->next) {
		struct server *srv;

		for (srv = px->srv; srv; srv = srv->next) {
			struct dns_resolvers *resolvers;

			if (!srv->resolvers_id)
				continue;

			if ((resolvers = find_resolvers_by_id(srv->resolvers_id)) == NULL) {
				ha_alert("config : %s '%s', server '%s': unable to find required resolvers '%s'\n",
					 proxy_type_str(px), px->id, srv->id, srv->resolvers_id);
				err_code |= (ERR_ALERT|ERR_ABORT);
				continue;
			}
			srv->resolvers = resolvers;

			if (srv->srvrq && !srv->srvrq->resolvers) {
				srv->srvrq->resolvers = srv->resolvers;
				if (dns_link_resolution(srv->srvrq, OBJ_TYPE_SRVRQ, 0) == -1) {
					ha_alert("config : %s '%s' : unable to set DNS resolution for server '%s'.\n",
						 proxy_type_str(px), px->id, srv->id);
					err_code |= (ERR_ALERT|ERR_ABORT);
					continue;
				}
			}
			if (dns_link_resolution(srv, OBJ_TYPE_SERVER, 0) == -1) {
				ha_alert("config : %s '%s', unable to set DNS resolution for server '%s'.\n",
					 proxy_type_str(px), px->id, srv->id);
				err_code |= (ERR_ALERT|ERR_ABORT);
				continue;
			}
		}
	}

	if (err_code & (ERR_ALERT|ERR_ABORT))
		goto err;

	return err_code;
  err:
	dns_deinit();
	return err_code;

}
