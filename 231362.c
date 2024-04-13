static int show_traces_open(struct inode *inode, struct file *file)
{
	struct trace_array *tr = inode->i_private;
	struct seq_file *m;
	int ret;

	if (tracing_disabled)
		return -ENODEV;

	ret = seq_open(file, &show_traces_seq_ops);
	if (ret)
		return ret;

	m = file->private_data;
	m->private = tr;

	return 0;
}