static int ep_send_events(struct eventpoll *ep,
			  struct epoll_event __user *events, int maxevents)
{
	struct ep_send_events_data esed;

	esed.maxevents = maxevents;
	esed.events = events;

	ep_scan_ready_list(ep, ep_send_events_proc, &esed, 0, false);
	return esed.res;
}