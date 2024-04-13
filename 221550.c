static void io_req_complete_post(struct io_kiocb *req, long res,
				 unsigned int cflags)
{
	struct io_ring_ctx *ctx = req->ctx;
	unsigned long flags;

	spin_lock_irqsave(&ctx->completion_lock, flags);
	__io_cqring_fill_event(ctx, req->user_data, res, cflags);
	/*
	 * If we're the last reference to this request, add to our locked
	 * free_list cache.
	 */
	if (req_ref_put_and_test(req)) {
		struct io_comp_state *cs = &ctx->submit_state.comp;

		if (req->flags & (REQ_F_LINK | REQ_F_HARDLINK)) {
			if (req->flags & (REQ_F_LINK_TIMEOUT | REQ_F_FAIL_LINK))
				io_disarm_next(req);
			if (req->link) {
				io_req_task_queue(req->link);
				req->link = NULL;
			}
		}
		io_dismantle_req(req);
		io_put_task(req->task, 1);
		list_add(&req->compl.list, &cs->locked_free_list);
		cs->locked_free_nr++;
	} else {
		if (!percpu_ref_tryget(&ctx->refs))
			req = NULL;
	}
	io_commit_cqring(ctx);
	spin_unlock_irqrestore(&ctx->completion_lock, flags);

	if (req) {
		io_cqring_ev_posted(ctx);
		percpu_ref_put(&ctx->refs);
	}
}