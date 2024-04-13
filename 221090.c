static __poll_t ep_eventpoll_poll(struct file *file, poll_table *wait)
{
	struct eventpoll *ep = file->private_data;
	int depth = 0;

	/* Insert inside our poll wait queue */
	poll_wait(file, &ep->poll_wait, wait);

	/*
	 * Proceed to find out if wanted events are really available inside
	 * the ready list.
	 */
	return ep_scan_ready_list(ep, ep_read_events_proc,
				  &depth, depth, false);
}