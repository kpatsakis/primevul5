static bool vhost_can_busy_poll(unsigned long endtime)
{
	return likely(!need_resched() && !time_after(busy_clock(), endtime) &&
		      !signal_pending(current));
}