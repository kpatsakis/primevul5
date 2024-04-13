
static enum hrtimer_restart io_link_timeout_fn(struct hrtimer *timer)
{
	struct io_timeout_data *data = container_of(timer,
						struct io_timeout_data, timer);
	struct io_kiocb *prev, *req = data->req;
	struct io_ring_ctx *ctx = req->ctx;
	unsigned long flags;

	spin_lock_irqsave(&ctx->completion_lock, flags);
	prev = req->timeout.head;
	req->timeout.head = NULL;

	/*
	 * We don't expect the list to be empty, that will only happen if we
	 * race with the completion of the linked work.
	 */
	if (prev && req_ref_inc_not_zero(prev))
		io_remove_next_linked(prev);
	else
		prev = NULL;
	spin_unlock_irqrestore(&ctx->completion_lock, flags);

	if (prev) {
		io_async_find_and_cancel(ctx, req, prev->user_data, -ETIME);
		io_put_req_deferred(prev, 1);
	} else {
		io_req_complete_post(req, -ETIME, 0);
	}
	io_put_req_deferred(req, 1);
	return HRTIMER_NORESTART;