static bool io_dismantle_req(struct io_kiocb *req)
{
	io_clean_op(req);

	if (req->io)
		kfree(req->io);
	if (req->file)
		io_put_file(req, req->file, (req->flags & REQ_F_FIXED_FILE));

	if (req->flags & REQ_F_INFLIGHT) {
		struct io_ring_ctx *ctx = req->ctx;
		unsigned long flags;

		spin_lock_irqsave(&ctx->inflight_lock, flags);
		list_del(&req->inflight_entry);
		if (waitqueue_active(&ctx->inflight_wait))
			wake_up(&ctx->inflight_wait);
		spin_unlock_irqrestore(&ctx->inflight_lock, flags);
	}

	return io_req_clean_work(req);
}