	__must_hold(&req->ctx->completion_lock)
{
	struct io_timeout_data *io = req->async_data;

	if (hrtimer_try_to_cancel(&io->timer) != -1) {
		atomic_set(&req->ctx->cq_timeouts,
			atomic_read(&req->ctx->cq_timeouts) + 1);
		list_del_init(&req->timeout.list);
		io_cqring_fill_event(req->ctx, req->user_data, status, 0);
		io_put_req_deferred(req, 1);
	}
}