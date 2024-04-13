static int dns_send_query(struct dns_resolution *resolution)
{
	struct dns_resolvers  *resolvers = resolution->resolvers;
	struct dns_nameserver *ns;

	list_for_each_entry(ns, &resolvers->nameservers, list) {
		int fd = ns->dgram->t.sock.fd;
		if (fd == -1) {
			if (dns_connect_namesaver(ns) == -1)
				continue;
			fd = ns->dgram->t.sock.fd;
			resolvers->nb_nameservers++;
		}
		fd_want_send(fd);
	}

	/* Update resolution */
	resolution->nb_queries   = 0;
	resolution->nb_responses = 0;
	resolution->last_query   = now_ms;

	/* Push the resolution at the end of the active list */
	LIST_DEL(&resolution->list);
	LIST_ADDQ(&resolvers->resolutions.curr, &resolution->list);
	return 0;
}
