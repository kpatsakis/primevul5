void tracer_tracing_on(struct trace_array *tr)
{
	if (tr->trace_buffer.buffer)
		ring_buffer_record_on(tr->trace_buffer.buffer);
	/*
	 * This flag is looked at when buffers haven't been allocated
	 * yet, or by some tracers (like irqsoff), that just want to
	 * know if the ring buffer has been disabled, but it can handle
	 * races of where it gets disabled but we still do a record.
	 * As the check is in the fast path of the tracers, it is more
	 * important to be fast than accurate.
	 */
	tr->buffer_disabled = 0;
	/* Make the flag seen by readers */
	smp_wmb();
}