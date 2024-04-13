
static void io_async_find_and_cancel(struct io_ring_ctx *ctx,
				     struct io_kiocb *req, __u64 sqe_addr,
				     int success_ret)
{
	unsigned long flags;
	int ret;

	ret = io_async_cancel_one(req->task->io_uring, sqe_addr, ctx);
	spin_lock_irqsave(&ctx->completion_lock, flags);
	if (ret != -ENOENT)
		goto done;
	ret = io_timeout_cancel(ctx, sqe_addr);
	if (ret != -ENOENT)
		goto done;
	ret = io_poll_cancel(ctx, sqe_addr, false);
done:
	if (!ret)
		ret = success_ret;
	io_cqring_fill_event(ctx, req->user_data, ret, 0);
	io_commit_cqring(ctx);
	spin_unlock_irqrestore(&ctx->completion_lock, flags);
	io_cqring_ev_posted(ctx);

	if (ret < 0)
		req_set_fail_links(req);