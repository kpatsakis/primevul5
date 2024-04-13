void disable_trace_on_warning(void)
{
	if (__disable_trace_on_warning)
		tracing_off();
}