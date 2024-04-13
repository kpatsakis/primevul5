static int allocate_trace_buffers(struct trace_array *tr, int size)
{
	int ret;

	ret = allocate_trace_buffer(tr, &tr->trace_buffer, size);
	if (ret)
		return ret;

#ifdef CONFIG_TRACER_MAX_TRACE
	ret = allocate_trace_buffer(tr, &tr->max_buffer,
				    allocate_snapshot ? size : 1);
	if (WARN_ON(ret)) {
		ring_buffer_free(tr->trace_buffer.buffer);
		tr->trace_buffer.buffer = NULL;
		free_percpu(tr->trace_buffer.data);
		tr->trace_buffer.data = NULL;
		return -ENOMEM;
	}
	tr->allocated_snapshot = allocate_snapshot;

	/*
	 * Only the top level trace array gets its snapshot allocated
	 * from the kernel command line.
	 */
	allocate_snapshot = false;
#endif
	return 0;
}