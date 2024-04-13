static void io_poll_remove_double(struct io_kiocb *req, void *data)
{
	struct io_poll_iocb *poll = data;

	lockdep_assert_held(&req->ctx->completion_lock);

	if (poll && poll->head) {
		struct wait_queue_head *head = poll->head;

		spin_lock(&head->lock);
		list_del_init(&poll->wait.entry);
		if (poll->wait.private)
			refcount_dec(&req->refs);
		poll->head = NULL;
		spin_unlock(&head->lock);
	}
}