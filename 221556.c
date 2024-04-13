static int io_timeout_cancel(struct io_ring_ctx *ctx, __u64 user_data)
	__must_hold(&ctx->completion_lock)
{
	struct io_kiocb *req = io_timeout_extract(ctx, user_data);

	if (IS_ERR(req))
		return PTR_ERR(req);

	req_set_fail_links(req);
	io_cqring_fill_event(ctx, req->user_data, -ECANCELED, 0);
	io_put_req_deferred(req, 1);
	return 0;