static int nfs4_delay_killable(long *timeout)
{
	might_sleep();

	freezable_schedule_timeout_killable_unsafe(
		nfs4_update_delay(timeout));
	if (!__fatal_signal_pending(current))
		return 0;
	return -EINTR;
}