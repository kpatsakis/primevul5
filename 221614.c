				 struct io_poll_iocb *poll, bool do_cancel)
	__must_hold(&req->ctx->completion_lock)
{
	bool do_complete = false;

	if (!poll->head)
		return false;
	spin_lock(&poll->head->lock);
	if (do_cancel)
		WRITE_ONCE(poll->canceled, true);
	if (!list_empty(&poll->wait.entry)) {
		list_del_init(&poll->wait.entry);
		do_complete = true;
	}
	spin_unlock(&poll->head->lock);
	hash_del(&req->hash_node);
	return do_complete;