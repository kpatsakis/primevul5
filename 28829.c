int dns_link_resolution(void *requester, int requester_type, int requester_locked)
{
	struct dns_resolution *res = NULL;
	struct dns_requester  *req;
	struct dns_resolvers  *resolvers;
	struct server         *srv   = NULL;
	struct dns_srvrq      *srvrq = NULL;
	char **hostname_dn;
	int   hostname_dn_len, query_type;

	switch (requester_type) {
		case OBJ_TYPE_SERVER:
			srv             = (struct server *)requester;
			hostname_dn     = &srv->hostname_dn;
			hostname_dn_len = srv->hostname_dn_len;
			resolvers       = srv->resolvers;
			query_type      = ((srv->dns_opts.family_prio == AF_INET)
					   ? DNS_RTYPE_A
					   : DNS_RTYPE_AAAA);
			break;

		case OBJ_TYPE_SRVRQ:
			srvrq           = (struct dns_srvrq *)requester;
			hostname_dn     = &srvrq->hostname_dn;
			hostname_dn_len = srvrq->hostname_dn_len;
			resolvers       = srvrq->resolvers;
			query_type      = DNS_RTYPE_SRV;
			break;

		default:
			goto err;
	}

	/* Get a resolution from the resolvers' wait queue or pool */
	if ((res = dns_pick_resolution(resolvers, hostname_dn, hostname_dn_len, query_type)) == NULL)
		goto err;

	if (srv) {
		if (!requester_locked)
			HA_SPIN_LOCK(SERVER_LOCK, &srv->lock);
		if (srv->dns_requester == NULL) {
			if ((req = calloc(1, sizeof(*req))) == NULL) {
				if (!requester_locked)
					HA_SPIN_UNLOCK(SERVER_LOCK, &srv->lock);
				goto err;
			}
			req->owner         = &srv->obj_type;
			srv->dns_requester = req;
		}
		else
			req = srv->dns_requester;
		if (!requester_locked)
			HA_SPIN_UNLOCK(SERVER_LOCK, &srv->lock);
	}
	else if (srvrq) {
		if (srvrq->dns_requester == NULL) {
			if ((req = calloc(1, sizeof(*req))) == NULL)
				goto err;
			req->owner           = &srvrq->obj_type;
			srvrq->dns_requester = req;
		}
		else
			req = srvrq->dns_requester;
	}
	else
		goto err;

	req->resolution         = res;
	req->requester_cb       = snr_resolution_cb;
	req->requester_error_cb = snr_resolution_error_cb;

	LIST_ADDQ(&res->requesters, &req->list);
	return 0;

  err:
	if (res && LIST_ISEMPTY(&res->requesters))
		dns_free_resolution(res);
	return -1;
}
