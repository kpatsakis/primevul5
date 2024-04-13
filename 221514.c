
static void io_queue_linked_timeout(struct io_kiocb *req)
{
	struct io_ring_ctx *ctx = req->ctx;

	spin_lock_irq(&ctx->completion_lock);
	/*
	 * If the back reference is NULL, then our linked request finished
	 * before we got a chance to setup the timer
	 */
	if (req->timeout.head) {
		struct io_timeout_data *data = req->async_data;

		data->timer.function = io_link_timeout_fn;
		hrtimer_start(&data->timer, timespec64_to_ktime(data->ts),
				data->mode);
	}
	spin_unlock_irq(&ctx->completion_lock);
	/* drop submission reference */
	io_put_req(req);