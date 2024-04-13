static void dns_update_resolvers_timeout(struct dns_resolvers *resolvers)
{
	struct dns_resolution *res;
	int next;

	next = tick_add(now_ms, resolvers->timeout.resolve);
	if (!LIST_ISEMPTY(&resolvers->resolutions.curr)) {
		res  = LIST_NEXT(&resolvers->resolutions.curr, struct dns_resolution *, list);
		next = MIN(next, tick_add(res->last_query, resolvers->timeout.retry));
	}

	list_for_each_entry(res, &resolvers->resolutions.wait, list)
		next = MIN(next, tick_add(res->last_resolution, dns_resolution_timeout(res)));

	resolvers->t->expire = next;
	task_queue(resolvers->t);
}
