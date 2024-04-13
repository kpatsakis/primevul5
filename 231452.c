static int wait_on_pipe(struct trace_iterator *iter, bool full)
{
	/* Iterators are static, they should be filled or empty */
	if (trace_buffer_iter(iter, iter->cpu_file))
		return 0;

	return ring_buffer_wait(iter->trace_buffer->buffer, iter->cpu_file,
				full);
}