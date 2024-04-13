dns_run_resolution(struct dns_resolution *resolution)
{
	struct dns_resolvers  *resolvers = resolution->resolvers;
	int query_id, i;

	/* Avoid sending requests for resolutions that don't yet have an
	 * hostname, ie resolutions linked to servers that do not yet have an
	 * fqdn */
	if (!resolution->hostname_dn)
		return 0;

	/* Check if a resolution has already been started for this server return
	 * directly to avoid resolution pill up. */
	if (resolution->step != RSLV_STEP_NONE)
		return 0;

	/* Generates a new query id. We try at most 100 times to find a free
	 * query id */
	for (i = 0; i < 100; ++i) {
		query_id = dns_rnd16();
		if (!eb32_lookup(&resolvers->query_ids, query_id))
			break;
		query_id = -1;
	}
	if (query_id == -1) {
		send_log(NULL, LOG_NOTICE,
			 "could not generate a query id for %s, in resolvers %s.\n",
			 resolution->hostname_dn, resolvers->id);
		return -1;
	}

	/* Update resolution parameters */
	resolution->query_id     = query_id;
	resolution->qid.key      = query_id;
	resolution->step         = RSLV_STEP_RUNNING;
	resolution->query_type   = resolution->prefered_query_type;
	resolution->try          = resolvers->resolve_retries;
	eb32_insert(&resolvers->query_ids, &resolution->qid);

	/* Send the DNS query */
	resolution->try -= 1;
	dns_send_query(resolution);
	return 1;
}
