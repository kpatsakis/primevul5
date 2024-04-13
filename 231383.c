void trace_init_global_iter(struct trace_iterator *iter)
{
	iter->tr = &global_trace;
	iter->trace = iter->tr->current_trace;
	iter->cpu_file = RING_BUFFER_ALL_CPUS;
	iter->trace_buffer = &global_trace.trace_buffer;

	if (iter->trace && iter->trace->open)
		iter->trace->open(iter);

	/* Annotate start of buffers if we had overruns */
	if (ring_buffer_overruns(iter->trace_buffer->buffer))
		iter->iter_flags |= TRACE_FILE_ANNOTATE;

	/* Output in nanoseconds only if we are using a clock in nanoseconds. */
	if (trace_clocks[iter->tr->clock_id].in_ns)
		iter->iter_flags |= TRACE_FILE_TIME_IN_NS;
}