static bool __io_cqring_overflow_flush(struct io_ring_ctx *ctx, bool force)
{
	struct io_rings *rings = ctx->rings;
	unsigned long flags;
	bool all_flushed, posted;

	if (!force && __io_cqring_events(ctx) == rings->cq_ring_entries)
		return false;

	posted = false;
	spin_lock_irqsave(&ctx->completion_lock, flags);
	while (!list_empty(&ctx->cq_overflow_list)) {
		struct io_uring_cqe *cqe = io_get_cqring(ctx);
		struct io_overflow_cqe *ocqe;

		if (!cqe && !force)
			break;
		ocqe = list_first_entry(&ctx->cq_overflow_list,
					struct io_overflow_cqe, list);
		if (cqe)
			memcpy(cqe, &ocqe->cqe, sizeof(*cqe));
		else
			WRITE_ONCE(ctx->rings->cq_overflow,
				   ++ctx->cached_cq_overflow);
		posted = true;
		list_del(&ocqe->list);
		kfree(ocqe);
	}

	all_flushed = list_empty(&ctx->cq_overflow_list);
	if (all_flushed) {
		clear_bit(0, &ctx->sq_check_overflow);
		clear_bit(0, &ctx->cq_check_overflow);
		ctx->rings->sq_flags &= ~IORING_SQ_CQ_OVERFLOW;
	}

	if (posted)
		io_commit_cqring(ctx);
	spin_unlock_irqrestore(&ctx->completion_lock, flags);
	if (posted)
		io_cqring_ev_posted(ctx);
	return all_flushed;
}