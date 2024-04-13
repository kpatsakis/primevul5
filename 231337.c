static int resize_buffer_duplicate_size(struct trace_buffer *trace_buf,
					struct trace_buffer *size_buf, int cpu_id)
{
	int cpu, ret = 0;

	if (cpu_id == RING_BUFFER_ALL_CPUS) {
		for_each_tracing_cpu(cpu) {
			ret = ring_buffer_resize(trace_buf->buffer,
				 per_cpu_ptr(size_buf->data, cpu)->entries, cpu);
			if (ret < 0)
				break;
			per_cpu_ptr(trace_buf->data, cpu)->entries =
				per_cpu_ptr(size_buf->data, cpu)->entries;
		}
	} else {
		ret = ring_buffer_resize(trace_buf->buffer,
				 per_cpu_ptr(size_buf->data, cpu_id)->entries, cpu_id);
		if (ret == 0)
			per_cpu_ptr(trace_buf->data, cpu_id)->entries =
				per_cpu_ptr(size_buf->data, cpu_id)->entries;
	}

	return ret;
}