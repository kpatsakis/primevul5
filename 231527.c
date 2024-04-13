static void disable_trace_buffered_event(void *data)
{
	this_cpu_inc(trace_buffered_event_cnt);
}