 */
void __io_uring_cancel(struct files_struct *files)
{
	struct io_uring_task *tctx = current->io_uring;
	DEFINE_WAIT(wait);
	s64 inflight;

	/* make sure overflow events are dropped */
	atomic_inc(&tctx->in_idle);
	do {
		/* read completions before cancelations */
		inflight = tctx_inflight(tctx, !!files);
		if (!inflight)
			break;
		io_uring_try_cancel(files);
		prepare_to_wait(&tctx->wait, &wait, TASK_UNINTERRUPTIBLE);

		/*
		 * If we've seen completions, retry without waiting. This
		 * avoids a race where a completion comes in before we did
		 * prepare_to_wait().
		 */
		if (inflight == tctx_inflight(tctx, !!files))
			schedule();
		finish_wait(&tctx->wait, &wait);
	} while (1);
	atomic_dec(&tctx->in_idle);

	io_uring_clean_tctx(tctx);
	if (!files) {
		/* for exec all current's requests should be gone, kill tctx */
		__io_uring_free(current);
	}