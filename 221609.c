
static int io_poll_double_wake(struct wait_queue_entry *wait, unsigned mode,
			       int sync, void *key)
{
	struct io_kiocb *req = wait->private;
	struct io_poll_iocb *poll = io_poll_get_single(req);
	__poll_t mask = key_to_poll(key);

	/* for instances that support it check for an event match first: */
	if (mask && !(mask & poll->events))
		return 0;
	if (!(poll->events & EPOLLONESHOT))
		return poll->wait.func(&poll->wait, mode, sync, key);

	list_del_init(&wait->entry);

	if (poll && poll->head) {
		bool done;

		spin_lock(&poll->head->lock);
		done = list_empty(&poll->wait.entry);
		if (!done)
			list_del_init(&poll->wait.entry);
		/* make sure double remove sees this as being gone */
		wait->private = NULL;
		spin_unlock(&poll->head->lock);
		if (!done) {
			/* use wait func handler, so it matches the rq type */
			poll->wait.func(&poll->wait, mode, sync, key);
		}
	}
	req_ref_put(req);
	return 1;