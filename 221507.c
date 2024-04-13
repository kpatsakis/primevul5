			  bool poll_only)
	__must_hold(&ctx->completion_lock)
{
	struct io_kiocb *req;

	req = io_poll_find(ctx, sqe_addr, poll_only);
	if (!req)
		return -ENOENT;
	if (io_poll_remove_one(req))
		return 0;

	return -EALREADY;