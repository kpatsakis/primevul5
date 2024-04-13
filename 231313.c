tracing_buffers_poll(struct file *filp, poll_table *poll_table)
{
	struct ftrace_buffer_info *info = filp->private_data;
	struct trace_iterator *iter = &info->iter;

	return trace_poll(iter, filp, poll_table);
}