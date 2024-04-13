static struct io_kiocb *__io_req_find_next(struct io_kiocb *req)
{
	struct io_kiocb *nxt;

	/*
	 * If LINK is set, we have dependent requests in this chain. If we
	 * didn't fail this request, queue the first one up, moving any other
	 * dependencies to the next request. In case of failure, fail the rest
	 * of the chain.
	 */
	if (req->flags & (REQ_F_LINK_TIMEOUT | REQ_F_FAIL_LINK)) {
		struct io_ring_ctx *ctx = req->ctx;
		unsigned long flags;
		bool posted;

		spin_lock_irqsave(&ctx->completion_lock, flags);
		posted = io_disarm_next(req);
		if (posted)
			io_commit_cqring(req->ctx);
		spin_unlock_irqrestore(&ctx->completion_lock, flags);
		if (posted)
			io_cqring_ev_posted(ctx);
	}
	nxt = req->link;
	req->link = NULL;
	return nxt;
}