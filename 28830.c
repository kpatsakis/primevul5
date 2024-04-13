static struct dns_resolution *dns_pick_resolution(struct dns_resolvers *resolvers,
						  char **hostname_dn, int hostname_dn_len,
						  int query_type)
{
	struct dns_resolution *res;

	if (!*hostname_dn)
		goto from_pool;

	/* Search for same hostname and query type in resolutions.curr */
	list_for_each_entry(res, &resolvers->resolutions.curr, list) {
		if (!res->hostname_dn)
			continue;
		if ((query_type == res->prefered_query_type) &&
		    hostname_dn_len == res->hostname_dn_len  &&
		    !memcmp(*hostname_dn, res->hostname_dn, hostname_dn_len))
			return res;
	}

	/* Search for same hostname and query type in resolutions.wait */
	list_for_each_entry(res, &resolvers->resolutions.wait, list) {
		if (!res->hostname_dn)
			continue;
		if ((query_type == res->prefered_query_type) &&
		    hostname_dn_len == res->hostname_dn_len  &&
		    !memcmp(*hostname_dn, res->hostname_dn, hostname_dn_len))
			return res;
	}

  from_pool:
	/* No resolution could be found, so let's allocate a new one */
	res = pool_alloc(dns_resolution_pool);
	if (res) {
		memset(res, 0, sizeof(*res));
		res->resolvers  = resolvers;
		res->uuid       = resolution_uuid;
		res->status     = RSLV_STATUS_NONE;
		res->step       = RSLV_STEP_NONE;
		res->last_valid = now_ms;

		LIST_INIT(&res->requesters);
		LIST_INIT(&res->response.answer_list);

		res->prefered_query_type = query_type;
		res->query_type          = query_type;
		res->hostname_dn         = *hostname_dn;
		res->hostname_dn_len     = hostname_dn_len;

		++resolution_uuid;

		/* Move the resolution to the resolvers wait queue */
		LIST_ADDQ(&resolvers->resolutions.wait, &res->list);
	}
	return res;
}
