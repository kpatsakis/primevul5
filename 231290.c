void tracing_stop(void)
{
	struct ring_buffer *buffer;
	unsigned long flags;

	raw_spin_lock_irqsave(&global_trace.start_lock, flags);
	if (global_trace.stop_count++)
		goto out;

	/* Prevent the buffers from switching */
	arch_spin_lock(&global_trace.max_lock);

	buffer = global_trace.trace_buffer.buffer;
	if (buffer)
		ring_buffer_record_disable(buffer);

#ifdef CONFIG_TRACER_MAX_TRACE
	buffer = global_trace.max_buffer.buffer;
	if (buffer)
		ring_buffer_record_disable(buffer);
#endif

	arch_spin_unlock(&global_trace.max_lock);

 out:
	raw_spin_unlock_irqrestore(&global_trace.start_lock, flags);
}