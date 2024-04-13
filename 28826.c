static void dns_free_resolution(struct dns_resolution *resolution)
{
	struct dns_requester *req, *reqback;

	/* clean up configuration */
	dns_reset_resolution(resolution);
	resolution->hostname_dn = NULL;
	resolution->hostname_dn_len = 0;

	list_for_each_entry_safe(req, reqback, &resolution->requesters, list) {
		LIST_DEL(&req->list);
		req->resolution = NULL;
	}

	LIST_DEL(&resolution->list);
	pool_free(dns_resolution_pool, resolution);
}
