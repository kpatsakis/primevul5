static int tracing_eval_map_open(struct inode *inode, struct file *filp)
{
	if (tracing_disabled)
		return -ENODEV;

	return seq_open(filp, &tracing_eval_map_seq_ops);
}