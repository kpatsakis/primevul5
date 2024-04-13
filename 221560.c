/* should only be called by SQPOLL task */
static void io_uring_cancel_sqpoll(struct io_sq_data *sqd)
{
	struct io_uring_task *tctx = current->io_uring;
	struct io_ring_ctx *ctx;
	s64 inflight;
	DEFINE_WAIT(wait);

	if (!current->io_uring)
		return;
	WARN_ON_ONCE(!sqd || sqd->thread != current);

	atomic_inc(&tctx->in_idle);
	do {
		/* read completions before cancelations */
		inflight = tctx_inflight(tctx, false);
		if (!inflight)
			break;
		list_for_each_entry(ctx, &sqd->ctx_list, sqd_list)
			io_uring_try_cancel_requests(ctx, current, NULL);

		prepare_to_wait(&tctx->wait, &wait, TASK_UNINTERRUPTIBLE);
		/*
		 * If we've seen completions, retry without waiting. This
		 * avoids a race where a completion comes in before we did
		 * prepare_to_wait().
		 */
		if (inflight == tctx_inflight(tctx, false))
			schedule();
		finish_wait(&tctx->wait, &wait);
	} while (1);
	atomic_dec(&tctx->in_idle);