static struct task *dns_process_resolvers(struct task *t, void *context, unsigned short state)
{
	struct dns_resolvers  *resolvers = context;
	struct dns_resolution *res, *resback;
	int exp;

	HA_SPIN_LOCK(DNS_LOCK, &resolvers->lock);

	/* Handle all expired resolutions from the active list */
	list_for_each_entry_safe(res, resback, &resolvers->resolutions.curr, list) {
		/* When we find the first resolution in the future, then we can
		 * stop here */
		exp = tick_add(res->last_query, resolvers->timeout.retry);
		if (!tick_is_expired(exp, now_ms))
			break;

		/* If current resolution has been tried too many times and
		 * finishes in timeout we update its status and remove it from
		 * the list */
		if (!res->try) {
			struct dns_requester *req;

			/* Notify the result to the requesters */
			if (!res->nb_responses)
				res->status = RSLV_STATUS_TIMEOUT;
			list_for_each_entry(req, &res->requesters, list)
				req->requester_error_cb(req, res->status);

			/* Clean up resolution info and remove it from the
			 * current list */
			dns_reset_resolution(res);
			LIST_DEL(&res->list);
			LIST_ADDQ(&resolvers->resolutions.wait, &res->list);
		}
		else {
			/* Otherwise resend the DNS query and requeue the resolution */
			if (!res->nb_responses || res->prefered_query_type != res->query_type) {
				/* No response received (a real timeout) or fallback already done */
				res->query_type = res->prefered_query_type;
				res->try--;
			}
			else {
				/* Fallback from A to AAAA or the opposite and re-send
				 * the resolution immediately. try counter is not
				 * decremented. */
				if (res->prefered_query_type == DNS_RTYPE_A)
					res->query_type = DNS_RTYPE_AAAA;
				else if (res->prefered_query_type == DNS_RTYPE_AAAA)
					res->query_type = DNS_RTYPE_A;
				else
					res->try--;
			}
			dns_send_query(res);
		}
	}

	/* Handle all resolutions in the wait list */
	list_for_each_entry_safe(res, resback, &resolvers->resolutions.wait, list) {
		exp = tick_add(res->last_resolution, dns_resolution_timeout(res));
		if (tick_isset(res->last_resolution) && !tick_is_expired(exp, now_ms))
			continue;

		if (dns_run_resolution(res) != 1) {
			res->last_resolution = now_ms;
			LIST_DEL(&res->list);
			LIST_ADDQ(&resolvers->resolutions.wait, &res->list);
		}
	}

	dns_update_resolvers_timeout(resolvers);
	HA_SPIN_UNLOCK(DNS_LOCK, &resolvers->lock);
	return t;
}
