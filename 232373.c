static int io_account_mem(struct io_ring_ctx *ctx, unsigned long nr_pages,
			  enum io_mem_account acct)
{
	int ret;

	if (ctx->limit_mem) {
		ret = __io_account_mem(ctx->user, nr_pages);
		if (ret)
			return ret;
	}

	if (ctx->sqo_mm) {
		if (acct == ACCT_LOCKED)
			ctx->sqo_mm->locked_vm += nr_pages;
		else if (acct == ACCT_PINNED)
			atomic64_add(nr_pages, &ctx->sqo_mm->pinned_vm);
	}

	return 0;
}