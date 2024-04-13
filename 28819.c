void dns_trigger_resolution(struct dns_requester *req)
{
	struct dns_resolvers  *resolvers;
	struct dns_resolution *res;
	int exp;

	if (!req || !req->resolution)
		return;
	res       = req->resolution;
	resolvers = res->resolvers;

	/* The resolution must not be triggered yet. Use the cached response, if
	 * valid */
	exp = tick_add(res->last_resolution, resolvers->hold.valid);
	if (resolvers->t && (res->status != RSLV_STATUS_VALID ||
	    !tick_isset(res->last_resolution) || tick_is_expired(exp, now_ms)))
		task_wakeup(resolvers->t, TASK_WOKEN_OTHER);
}
