static void io_poll_remove_double(struct io_kiocb *req)
	__must_hold(&req->ctx->completion_lock)
{
	struct io_poll_iocb *poll = io_poll_get_double(req);

	lockdep_assert_held(&req->ctx->completion_lock);

	if (poll && poll->head) {
		struct wait_queue_head *head = poll->head;

		spin_lock(&head->lock);
		list_del_init(&poll->wait.entry);
		if (poll->wait.private)
			req_ref_put(req);
		poll->head = NULL;
		spin_unlock(&head->lock);
	}