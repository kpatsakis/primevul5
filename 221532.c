static int io_iopoll_check(struct io_ring_ctx *ctx, long min)
{
	unsigned int nr_events = 0;
	int ret = 0;

	/*
	 * We disallow the app entering submit/complete with polling, but we
	 * still need to lock the ring to prevent racing with polled issue
	 * that got punted to a workqueue.
	 */
	mutex_lock(&ctx->uring_lock);
	/*
	 * Don't enter poll loop if we already have events pending.
	 * If we do, we can potentially be spinning for commands that
	 * already triggered a CQE (eg in error).
	 */
	if (test_bit(0, &ctx->cq_check_overflow))
		__io_cqring_overflow_flush(ctx, false);
	if (io_cqring_events(ctx))
		goto out;
	do {
		/*
		 * If a submit got punted to a workqueue, we can have the
		 * application entering polling for a command before it gets
		 * issued. That app will hold the uring_lock for the duration
		 * of the poll right here, so we need to take a breather every
		 * now and then to ensure that the issue has a chance to add
		 * the poll to the issued list. Otherwise we can spin here
		 * forever, while the workqueue is stuck trying to acquire the
		 * very same mutex.
		 */
		if (list_empty(&ctx->iopoll_list)) {
			mutex_unlock(&ctx->uring_lock);
			io_run_task_work();
			mutex_lock(&ctx->uring_lock);

			if (list_empty(&ctx->iopoll_list))
				break;
		}
		ret = io_do_iopoll(ctx, &nr_events, min);
	} while (!ret && nr_events < min && !need_resched());
out:
	mutex_unlock(&ctx->uring_lock);
	return ret;
}