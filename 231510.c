static int tracing_buffers_open(struct inode *inode, struct file *filp)
{
	struct trace_array *tr = inode->i_private;
	struct ftrace_buffer_info *info;
	int ret;

	if (tracing_disabled)
		return -ENODEV;

	if (trace_array_get(tr) < 0)
		return -ENODEV;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info) {
		trace_array_put(tr);
		return -ENOMEM;
	}

	mutex_lock(&trace_types_lock);

	info->iter.tr		= tr;
	info->iter.cpu_file	= tracing_get_cpu(inode);
	info->iter.trace	= tr->current_trace;
	info->iter.trace_buffer = &tr->trace_buffer;
	info->spare		= NULL;
	/* Force reading ring buffer for first read */
	info->read		= (unsigned int)-1;

	filp->private_data = info;

	tr->current_trace->ref++;

	mutex_unlock(&trace_types_lock);

	ret = nonseekable_open(inode, filp);
	if (ret < 0)
		trace_array_put(tr);

	return ret;
}