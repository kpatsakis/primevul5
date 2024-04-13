
static int io_submit_sqes(struct io_ring_ctx *ctx, unsigned int nr)
{
	int submitted = 0;

	/* make sure SQ entry isn't read before tail */
	nr = min3(nr, ctx->sq_entries, io_sqring_entries(ctx));

	if (!percpu_ref_tryget_many(&ctx->refs, nr))
		return -EAGAIN;

	percpu_counter_add(&current->io_uring->inflight, nr);
	refcount_add(nr, &current->usage);
	io_submit_state_start(&ctx->submit_state, nr);

	while (submitted < nr) {
		const struct io_uring_sqe *sqe;
		struct io_kiocb *req;

		req = io_alloc_req(ctx);
		if (unlikely(!req)) {
			if (!submitted)
				submitted = -EAGAIN;
			break;
		}
		sqe = io_get_sqe(ctx);
		if (unlikely(!sqe)) {
			kmem_cache_free(req_cachep, req);
			break;
		}
		/* will complete beyond this point, count as submitted */
		submitted++;
		if (io_submit_sqe(ctx, req, sqe))
			break;
	}

	if (unlikely(submitted != nr)) {
		int ref_used = (submitted == -EAGAIN) ? 0 : submitted;
		struct io_uring_task *tctx = current->io_uring;
		int unused = nr - ref_used;

		percpu_ref_put_many(&ctx->refs, unused);
		percpu_counter_sub(&tctx->inflight, unused);
		put_task_struct_many(current, unused);
	}

	io_submit_state_end(&ctx->submit_state, ctx);
	 /* Commit SQ ring head once we've consumed and submitted all SQEs */
	io_commit_sqring(ctx);

	return submitted;