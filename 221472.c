
static int io_async_cancel(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_ring_ctx *ctx = req->ctx;
	u64 sqe_addr = req->cancel.addr;
	struct io_tctx_node *node;
	int ret;

	/* tasks should wait for their io-wq threads, so safe w/o sync */
	ret = io_async_cancel_one(req->task->io_uring, sqe_addr, ctx);
	spin_lock_irq(&ctx->completion_lock);
	if (ret != -ENOENT)
		goto done;
	ret = io_timeout_cancel(ctx, sqe_addr);
	if (ret != -ENOENT)
		goto done;
	ret = io_poll_cancel(ctx, sqe_addr, false);
	if (ret != -ENOENT)
		goto done;
	spin_unlock_irq(&ctx->completion_lock);

	/* slow path, try all io-wq's */
	io_ring_submit_lock(ctx, !(issue_flags & IO_URING_F_NONBLOCK));
	ret = -ENOENT;
	list_for_each_entry(node, &ctx->tctx_list, ctx_node) {
		struct io_uring_task *tctx = node->task->io_uring;

		ret = io_async_cancel_one(tctx, req->cancel.addr, ctx);
		if (ret != -ENOENT)
			break;
	}
	io_ring_submit_unlock(ctx, !(issue_flags & IO_URING_F_NONBLOCK));

	spin_lock_irq(&ctx->completion_lock);
done:
	io_cqring_fill_event(ctx, req->user_data, ret, 0);
	io_commit_cqring(ctx);
	spin_unlock_irq(&ctx->completion_lock);
	io_cqring_ev_posted(ctx);

	if (ret < 0)
		req_set_fail_links(req);
	io_put_req(req);
	return 0;