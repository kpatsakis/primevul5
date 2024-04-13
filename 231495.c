int tracing_is_on(void)
{
	return tracer_tracing_is_on(&global_trace);
}