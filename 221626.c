static void io_sq_thread_park(struct io_sq_data *sqd)
	__acquires(&sqd->lock)
{
	WARN_ON_ONCE(sqd->thread == current);

	atomic_inc(&sqd->park_pending);
	set_bit(IO_SQ_THREAD_SHOULD_PARK, &sqd->state);
	mutex_lock(&sqd->lock);
	if (sqd->thread)
		wake_up_process(sqd->thread);