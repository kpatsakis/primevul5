int tracer_tracing_is_on(struct trace_array *tr)
{
	if (tr->trace_buffer.buffer)
		return ring_buffer_record_is_on(tr->trace_buffer.buffer);
	return !tr->buffer_disabled;
}