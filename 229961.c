static int nfs4_delay_interruptible(long *timeout)
{
	might_sleep();

	freezable_schedule_timeout_interruptible_unsafe(nfs4_update_delay(timeout));
	if (!signal_pending(current))
		return 0;
	return __fatal_signal_pending(current) ? -EINTR :-ERESTARTSYS;
}