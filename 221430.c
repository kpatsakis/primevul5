	__must_hold(&req->ctx->completion_lock)
{
	struct io_kiocb *link = req->link;

	/*
	 * Can happen if a linked timeout fired and link had been like
	 * req -> link t-out -> link t-out [-> ...]
	 */
	if (link && (link->flags & REQ_F_LTIMEOUT_ACTIVE)) {
		struct io_timeout_data *io = link->async_data;

		io_remove_next_linked(req);
		link->timeout.head = NULL;
		if (hrtimer_try_to_cancel(&io->timer) != -1) {
			io_cqring_fill_event(link->ctx, link->user_data,
					     -ECANCELED, 0);
			io_put_req_deferred(link, 1);
			return true;
		}
	}
	return false;
}