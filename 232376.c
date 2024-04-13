static void io_ring_ctx_wait_and_kill(struct io_ring_ctx *ctx)
{
	mutex_lock(&ctx->uring_lock);
	percpu_ref_kill(&ctx->refs);
	mutex_unlock(&ctx->uring_lock);

	io_kill_timeouts(ctx);
	io_poll_remove_all(ctx);

	if (ctx->io_wq)
		io_wq_cancel_all(ctx->io_wq);

	/* if we failed setting up the ctx, we might not have any rings */
	if (ctx->rings)
		io_cqring_overflow_flush(ctx, true);
	io_iopoll_try_reap_events(ctx);
	idr_for_each(&ctx->personality_idr, io_remove_personalities, ctx);

	/*
	 * Do this upfront, so we won't have a grace period where the ring
	 * is closed but resources aren't reaped yet. This can cause
	 * spurious failure in setting up a new ring.
	 */
	io_unaccount_mem(ctx, ring_pages(ctx->sq_entries, ctx->cq_entries),
			 ACCT_LOCKED);

	INIT_WORK(&ctx->exit_work, io_ring_exit_work);
	queue_work(system_wq, &ctx->exit_work);
}