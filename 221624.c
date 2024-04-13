static void io_submit_flush_completions(struct io_comp_state *cs,
					struct io_ring_ctx *ctx)
{
	int i, nr = cs->nr;
	struct io_kiocb *req;
	struct req_batch rb;

	io_init_req_batch(&rb);
	spin_lock_irq(&ctx->completion_lock);
	for (i = 0; i < nr; i++) {
		req = cs->reqs[i];
		__io_cqring_fill_event(ctx, req->user_data, req->result,
					req->compl.cflags);
	}
	io_commit_cqring(ctx);
	spin_unlock_irq(&ctx->completion_lock);

	io_cqring_ev_posted(ctx);
	for (i = 0; i < nr; i++) {
		req = cs->reqs[i];

		/* submission and completion refs */
		if (req_ref_sub_and_test(req, 2))
			io_req_free_batch(&rb, req, &ctx->submit_state);
	}

	io_req_free_batch_finish(ctx, &rb);
	cs->nr = 0;
}