void tracing_reset(struct trace_buffer *buf, int cpu)
{
	struct ring_buffer *buffer = buf->buffer;

	if (!buffer)
		return;

	ring_buffer_record_disable(buffer);

	/* Make sure all commits have finished */
	synchronize_sched();
	ring_buffer_reset_cpu(buffer, cpu);

	ring_buffer_record_enable(buffer);
}