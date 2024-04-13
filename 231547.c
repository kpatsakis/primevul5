void tracing_reset_online_cpus(struct trace_buffer *buf)
{
	struct ring_buffer *buffer = buf->buffer;
	int cpu;

	if (!buffer)
		return;

	ring_buffer_record_disable(buffer);

	/* Make sure all commits have finished */
	synchronize_sched();

	buf->time_start = buffer_ftrace_now(buf, buf->cpu);

	for_each_online_cpu(cpu)
		ring_buffer_reset_cpu(buffer, cpu);

	ring_buffer_record_enable(buffer);
}