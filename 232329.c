static bool io_cqring_overflow_flush(struct io_ring_ctx *ctx, bool force)
{
	struct io_rings *rings = ctx->rings;
	struct io_uring_cqe *cqe;
	struct io_kiocb *req;
	unsigned long flags;
	LIST_HEAD(list);

	if (!force) {
		if (list_empty_careful(&ctx->cq_overflow_list))
			return true;
		if ((ctx->cached_cq_tail - READ_ONCE(rings->cq.head) ==
		    rings->cq_ring_entries))
			return false;
	}

	spin_lock_irqsave(&ctx->completion_lock, flags);

	/* if force is set, the ring is going away. always drop after that */
	if (force)
		ctx->cq_overflow_flushed = 1;

	cqe = NULL;
	while (!list_empty(&ctx->cq_overflow_list)) {
		cqe = io_get_cqring(ctx);
		if (!cqe && !force)
			break;

		req = list_first_entry(&ctx->cq_overflow_list, struct io_kiocb,
						compl.list);
		list_move(&req->compl.list, &list);
		req->flags &= ~REQ_F_OVERFLOW;
		if (cqe) {
			WRITE_ONCE(cqe->user_data, req->user_data);
			WRITE_ONCE(cqe->res, req->result);
			WRITE_ONCE(cqe->flags, req->compl.cflags);
		} else {
			WRITE_ONCE(ctx->rings->cq_overflow,
				atomic_inc_return(&ctx->cached_cq_overflow));
		}
	}

	io_commit_cqring(ctx);
	io_cqring_mark_overflow(ctx);

	spin_unlock_irqrestore(&ctx->completion_lock, flags);
	io_cqring_ev_posted(ctx);

	while (!list_empty(&list)) {
		req = list_first_entry(&list, struct io_kiocb, compl.list);
		list_del(&req->compl.list);
		io_put_req(req);
	}

	return cqe != NULL;
}