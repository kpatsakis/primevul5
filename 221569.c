				      wait_queue_func_t wake_func)
	__acquires(&ctx->completion_lock)
{
	struct io_ring_ctx *ctx = req->ctx;
	bool cancel = false;

	INIT_HLIST_NODE(&req->hash_node);
	io_init_poll_iocb(poll, mask, wake_func);
	poll->file = req->file;
	poll->wait.private = req;

	ipt->pt._key = mask;
	ipt->req = req;
	ipt->error = -EINVAL;

	mask = vfs_poll(req->file, &ipt->pt) & poll->events;

	spin_lock_irq(&ctx->completion_lock);
	if (likely(poll->head)) {
		spin_lock(&poll->head->lock);
		if (unlikely(list_empty(&poll->wait.entry))) {
			if (ipt->error)
				cancel = true;
			ipt->error = 0;
			mask = 0;
		}
		if ((mask && (poll->events & EPOLLONESHOT)) || ipt->error)
			list_del_init(&poll->wait.entry);
		else if (cancel)
			WRITE_ONCE(poll->canceled, true);
		else if (!poll->done) /* actually waiting for an event */
			io_poll_req_insert(req);
		spin_unlock(&poll->head->lock);
	}

	return mask;