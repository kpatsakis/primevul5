
static void io_tctx_exit_cb(struct callback_head *cb)
{
	struct io_uring_task *tctx = current->io_uring;
	struct io_tctx_exit *work;

	work = container_of(cb, struct io_tctx_exit, task_work);
	/*
	 * When @in_idle, we're in cancellation and it's racy to remove the
	 * node. It'll be removed by the end of cancellation, just ignore it.
	 */
	if (!atomic_read(&tctx->in_idle))
		io_uring_del_task_file((unsigned long)work->ctx);
	complete(&work->completion);