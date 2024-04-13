static void tracing_start_tr(struct trace_array *tr)
{
	struct ring_buffer *buffer;
	unsigned long flags;

	if (tracing_disabled)
		return;

	/* If global, we need to also start the max tracer */
	if (tr->flags & TRACE_ARRAY_FL_GLOBAL)
		return tracing_start();

	raw_spin_lock_irqsave(&tr->start_lock, flags);

	if (--tr->stop_count) {
		if (tr->stop_count < 0) {
			/* Someone screwed up their debugging */
			WARN_ON_ONCE(1);
			tr->stop_count = 0;
		}
		goto out;
	}

	buffer = tr->trace_buffer.buffer;
	if (buffer)
		ring_buffer_record_enable(buffer);

 out:
	raw_spin_unlock_irqrestore(&tr->start_lock, flags);
}