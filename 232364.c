static void io_uring_cancel_files(struct io_ring_ctx *ctx,
				  struct files_struct *files)
{
	if (list_empty_careful(&ctx->inflight_list))
		return;

	/* cancel all at once, should be faster than doing it one by one*/
	io_wq_cancel_cb(ctx->io_wq, io_wq_files_match, files, true);

	while (!list_empty_careful(&ctx->inflight_list)) {
		struct io_kiocb *cancel_req = NULL, *req;
		DEFINE_WAIT(wait);

		spin_lock_irq(&ctx->inflight_lock);
		list_for_each_entry(req, &ctx->inflight_list, inflight_entry) {
			if (req->work.files != files)
				continue;
			/* req is being completed, ignore */
			if (!refcount_inc_not_zero(&req->refs))
				continue;
			cancel_req = req;
			break;
		}
		if (cancel_req)
			prepare_to_wait(&ctx->inflight_wait, &wait,
						TASK_UNINTERRUPTIBLE);
		spin_unlock_irq(&ctx->inflight_lock);

		/* We need to keep going until we don't find a matching req */
		if (!cancel_req)
			break;

		if (cancel_req->flags & REQ_F_OVERFLOW) {
			spin_lock_irq(&ctx->completion_lock);
			list_del(&cancel_req->compl.list);
			cancel_req->flags &= ~REQ_F_OVERFLOW;

			io_cqring_mark_overflow(ctx);
			WRITE_ONCE(ctx->rings->cq_overflow,
				atomic_inc_return(&ctx->cached_cq_overflow));
			io_commit_cqring(ctx);
			spin_unlock_irq(&ctx->completion_lock);

			/*
			 * Put inflight ref and overflow ref. If that's
			 * all we had, then we're done with this request.
			 */
			if (refcount_sub_and_test(2, &cancel_req->refs)) {
				io_free_req(cancel_req);
				finish_wait(&ctx->inflight_wait, &wait);
				continue;
			}
		} else {
			io_wq_cancel_work(ctx->io_wq, &cancel_req->work);
			io_put_req(cancel_req);
		}

		schedule();
		finish_wait(&ctx->inflight_wait, &wait);
	}
}