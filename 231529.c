static int tracing_single_release_tr(struct inode *inode, struct file *file)
{
	struct trace_array *tr = inode->i_private;

	trace_array_put(tr);

	return single_release(inode, file);
}