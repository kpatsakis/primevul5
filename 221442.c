static bool io_cqring_overflow_flush(struct io_ring_ctx *ctx, bool force)
{
	bool ret = true;

	if (test_bit(0, &ctx->cq_check_overflow)) {
		/* iopoll syncs against uring_lock, not completion_lock */
		if (ctx->flags & IORING_SETUP_IOPOLL)
			mutex_lock(&ctx->uring_lock);
		ret = __io_cqring_overflow_flush(ctx, force);
		if (ctx->flags & IORING_SETUP_IOPOLL)
			mutex_unlock(&ctx->uring_lock);
	}

	return ret;
}