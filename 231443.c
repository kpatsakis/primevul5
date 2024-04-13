static int __tracing_resize_ring_buffer(struct trace_array *tr,
					unsigned long size, int cpu)
{
	int ret;

	/*
	 * If kernel or user changes the size of the ring buffer
	 * we use the size that was given, and we can forget about
	 * expanding it later.
	 */
	ring_buffer_expanded = true;

	/* May be called before buffers are initialized */
	if (!tr->trace_buffer.buffer)
		return 0;

	ret = ring_buffer_resize(tr->trace_buffer.buffer, size, cpu);
	if (ret < 0)
		return ret;

#ifdef CONFIG_TRACER_MAX_TRACE
	if (!(tr->flags & TRACE_ARRAY_FL_GLOBAL) ||
	    !tr->current_trace->use_max_tr)
		goto out;

	ret = ring_buffer_resize(tr->max_buffer.buffer, size, cpu);
	if (ret < 0) {
		int r = resize_buffer_duplicate_size(&tr->trace_buffer,
						     &tr->trace_buffer, cpu);
		if (r < 0) {
			/*
			 * AARGH! We are left with different
			 * size max buffer!!!!
			 * The max buffer is our "snapshot" buffer.
			 * When a tracer needs a snapshot (one of the
			 * latency tracers), it swaps the max buffer
			 * with the saved snap shot. We succeeded to
			 * update the size of the main buffer, but failed to
			 * update the size of the max buffer. But when we tried
			 * to reset the main buffer to the original size, we
			 * failed there too. This is very unlikely to
			 * happen, but if it does, warn and kill all
			 * tracing.
			 */
			WARN_ON(1);
			tracing_disabled = 1;
		}
		return ret;
	}

	if (cpu == RING_BUFFER_ALL_CPUS)
		set_buffer_entries(&tr->max_buffer, size);
	else
		per_cpu_ptr(tr->max_buffer.data, cpu)->entries = size;

 out:
#endif /* CONFIG_TRACER_MAX_TRACE */

	if (cpu == RING_BUFFER_ALL_CPUS)
		set_buffer_entries(&tr->trace_buffer, size);
	else
		per_cpu_ptr(tr->trace_buffer.data, cpu)->entries = size;

	return ret;
}