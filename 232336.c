static void io_ring_exit_work(struct work_struct *work)
{
	struct io_ring_ctx *ctx = container_of(work, struct io_ring_ctx,
					       exit_work);

	/*
	 * If we're doing polled IO and end up having requests being
	 * submitted async (out-of-line), then completions can come in while
	 * we're waiting for refs to drop. We need to reap these manually,
	 * as nobody else will be looking for them.
	 */
	do {
		if (ctx->rings)
			io_cqring_overflow_flush(ctx, true);
		io_iopoll_try_reap_events(ctx);
	} while (!wait_for_completion_timeout(&ctx->ref_comp, HZ/20));
	io_ring_ctx_free(ctx);
}