static int __io_sq_thread_acquire_mm(struct io_ring_ctx *ctx)
{
	if (!current->mm) {
		if (unlikely(!(ctx->flags & IORING_SETUP_SQPOLL) ||
			     !mmget_not_zero(ctx->sqo_mm)))
			return -EFAULT;
		kthread_use_mm(ctx->sqo_mm);
	}

	return 0;
}