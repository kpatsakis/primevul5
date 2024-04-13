static bool ep_busy_loop_end(void *p, unsigned long start_time)
{
	struct eventpoll *ep = p;

	return ep_events_available(ep) || busy_loop_timeout(start_time);
}