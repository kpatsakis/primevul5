static void free_snapshot(struct trace_array *tr)
{
	/*
	 * We don't free the ring buffer. instead, resize it because
	 * The max_tr ring buffer has some state (e.g. ring->clock) and
	 * we want preserve it.
	 */
	ring_buffer_resize(tr->max_buffer.buffer, 1, RING_BUFFER_ALL_CPUS);
	set_buffer_entries(&tr->max_buffer, 1);
	tracing_reset_online_cpus(&tr->max_buffer);
	tr->allocated_snapshot = false;
}