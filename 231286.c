static int tracing_open_generic_tr(struct inode *inode, struct file *filp)
{
	struct trace_array *tr = inode->i_private;

	if (tracing_disabled)
		return -ENODEV;

	if (trace_array_get(tr) < 0)
		return -ENODEV;

	filp->private_data = inode->i_private;

	return 0;
}