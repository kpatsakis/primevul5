static int __init set_trace_boot_options(char *str)
{
	strlcpy(trace_boot_options_buf, str, MAX_TRACER_SIZE);
	return 0;
}