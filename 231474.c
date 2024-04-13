static void free_trace_buffers(struct trace_array *tr)
{
	if (!tr)
		return;

	free_trace_buffer(&tr->trace_buffer);

#ifdef CONFIG_TRACER_MAX_TRACE
	free_trace_buffer(&tr->max_buffer);
#endif
}