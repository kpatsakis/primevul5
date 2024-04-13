trace_poll(struct trace_iterator *iter, struct file *filp, poll_table *poll_table)
{
	struct trace_array *tr = iter->tr;

	/* Iterators are static, they should be filled or empty */
	if (trace_buffer_iter(iter, iter->cpu_file))
		return POLLIN | POLLRDNORM;

	if (tr->trace_flags & TRACE_ITER_BLOCK)
		/*
		 * Always select as readable when in blocking mode
		 */
		return POLLIN | POLLRDNORM;
	else
		return ring_buffer_poll_wait(iter->trace_buffer->buffer, iter->cpu_file,
					     filp, poll_table);
}