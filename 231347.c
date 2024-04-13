static int tracing_set_clock(struct trace_array *tr, const char *clockstr)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(trace_clocks); i++) {
		if (strcmp(trace_clocks[i].name, clockstr) == 0)
			break;
	}
	if (i == ARRAY_SIZE(trace_clocks))
		return -EINVAL;

	mutex_lock(&trace_types_lock);

	tr->clock_id = i;

	ring_buffer_set_clock(tr->trace_buffer.buffer, trace_clocks[i].func);

	/*
	 * New clock may not be consistent with the previous clock.
	 * Reset the buffer so that it doesn't have incomparable timestamps.
	 */
	tracing_reset_online_cpus(&tr->trace_buffer);

#ifdef CONFIG_TRACER_MAX_TRACE
	if (tr->max_buffer.buffer)
		ring_buffer_set_clock(tr->max_buffer.buffer, trace_clocks[i].func);
	tracing_reset_online_cpus(&tr->max_buffer);
#endif

	mutex_unlock(&trace_types_lock);

	return 0;
}