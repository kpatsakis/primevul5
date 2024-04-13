static int tracing_saved_cmdlines_open(struct inode *inode, struct file *filp)
{
	if (tracing_disabled)
		return -ENODEV;

	return seq_open(filp, &tracing_saved_cmdlines_seq_ops);
}