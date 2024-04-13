static int snapshot_raw_open(struct inode *inode, struct file *filp)
{
	struct ftrace_buffer_info *info;
	int ret;

	ret = tracing_buffers_open(inode, filp);
	if (ret < 0)
		return ret;

	info = filp->private_data;

	if (info->iter.trace->use_max_tr) {
		tracing_buffers_release(inode, filp);
		return -EBUSY;
	}

	info->iter.snapshot = true;
	info->iter.trace_buffer = &info->iter.tr->max_buffer;

	return ret;
}