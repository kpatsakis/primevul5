
static int io_run_task_work_sig(void)
{
	if (io_run_task_work())
		return 1;
	if (!signal_pending(current))
		return 0;
	if (test_thread_flag(TIF_NOTIFY_SIGNAL))
		return -ERESTARTSYS;
	return -EINTR;