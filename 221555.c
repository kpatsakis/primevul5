
static void io_sq_thread_stop(struct io_sq_data *sqd)
{
	WARN_ON_ONCE(sqd->thread == current);
	WARN_ON_ONCE(test_bit(IO_SQ_THREAD_SHOULD_STOP, &sqd->state));

	set_bit(IO_SQ_THREAD_SHOULD_STOP, &sqd->state);
	mutex_lock(&sqd->lock);
	if (sqd->thread)
		wake_up_process(sqd->thread);
	mutex_unlock(&sqd->lock);
	wait_for_completion(&sqd->exited);