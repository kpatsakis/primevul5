static void tracing_stop_tr(struct trace_array *tr)
{
	struct ring_buffer *buffer;
	unsigned long flags;

	/* If global, we need to also stop the max tracer */
	if (tr->flags & TRACE_ARRAY_FL_GLOBAL)
		return tracing_stop();

	raw_spin_lock_irqsave(&tr->start_lock, flags);
	if (tr->stop_count++)
		goto out;

	buffer = tr->trace_buffer.buffer;
	if (buffer)
		ring_buffer_record_disable(buffer);

 out:
	raw_spin_unlock_irqrestore(&tr->start_lock, flags);
}