static int __init set_trace_boot_clock(char *str)
{
	strlcpy(trace_boot_clock_buf, str, MAX_TRACER_SIZE);
	trace_boot_clock = trace_boot_clock_buf;
	return 0;
}