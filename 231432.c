static int tracing_buffers_release(struct inode *inode, struct file *file)
{
	struct ftrace_buffer_info *info = file->private_data;
	struct trace_iterator *iter = &info->iter;

	mutex_lock(&trace_types_lock);

	iter->tr->current_trace->ref--;

	__trace_array_put(iter->tr);

	if (info->spare)
		ring_buffer_free_read_page(iter->trace_buffer->buffer,
					   info->spare_cpu, info->spare);
	kfree(info);

	mutex_unlock(&trace_types_lock);

	return 0;
}