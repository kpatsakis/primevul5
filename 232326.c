static void io_unaccount_mem(struct io_ring_ctx *ctx, unsigned long nr_pages,
			     enum io_mem_account acct)
{
	if (ctx->limit_mem)
		__io_unaccount_mem(ctx->user, nr_pages);

	if (ctx->sqo_mm) {
		if (acct == ACCT_LOCKED)
			ctx->sqo_mm->locked_vm -= nr_pages;
		else if (acct == ACCT_PINNED)
			atomic64_sub(nr_pages, &ctx->sqo_mm->pinned_vm);
	}
}