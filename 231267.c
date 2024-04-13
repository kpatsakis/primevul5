int trace_empty(struct trace_iterator *iter)
{
	struct ring_buffer_iter *buf_iter;
	int cpu;

	/* If we are looking at one CPU buffer, only check that one */
	if (iter->cpu_file != RING_BUFFER_ALL_CPUS) {
		cpu = iter->cpu_file;
		buf_iter = trace_buffer_iter(iter, cpu);
		if (buf_iter) {
			if (!ring_buffer_iter_empty(buf_iter))
				return 0;
		} else {
			if (!ring_buffer_empty_cpu(iter->trace_buffer->buffer, cpu))
				return 0;
		}
		return 1;
	}

	for_each_tracing_cpu(cpu) {
		buf_iter = trace_buffer_iter(iter, cpu);
		if (buf_iter) {
			if (!ring_buffer_iter_empty(buf_iter))
				return 0;
		} else {
			if (!ring_buffer_empty_cpu(iter->trace_buffer->buffer, cpu))
				return 0;
		}
	}

	return 1;
}