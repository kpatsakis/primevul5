
static void __io_queue_proc(struct io_poll_iocb *poll, struct io_poll_table *pt,
			    struct wait_queue_head *head,
			    struct io_poll_iocb **poll_ptr)
{
	struct io_kiocb *req = pt->req;

	/*
	 * If poll->head is already set, it's because the file being polled
	 * uses multiple waitqueues for poll handling (eg one for read, one
	 * for write). Setup a separate io_poll_iocb if this happens.
	 */
	if (unlikely(poll->head)) {
		struct io_poll_iocb *poll_one = poll;

		/* already have a 2nd entry, fail a third attempt */
		if (*poll_ptr) {
			pt->error = -EINVAL;
			return;
		}
		/*
		 * Can't handle multishot for double wait for now, turn it
		 * into one-shot mode.
		 */
		if (!(req->poll.events & EPOLLONESHOT))
			req->poll.events |= EPOLLONESHOT;
		/* double add on the same waitqueue head, ignore */
		if (poll->head == head)
			return;
		poll = kmalloc(sizeof(*poll), GFP_ATOMIC);
		if (!poll) {
			pt->error = -ENOMEM;
			return;
		}
		io_init_poll_iocb(poll, poll_one->events, io_poll_double_wake);
		req_ref_get(req);
		poll->wait.private = req;
		*poll_ptr = poll;
	}

	pt->error = 0;
	poll->head = head;

	if (poll->events & EPOLLEXCLUSIVE)
		add_wait_queue_exclusive(head, &poll->wait);
	else
		add_wait_queue(head, &poll->wait);