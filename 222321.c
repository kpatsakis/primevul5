static int input_proc_handlers_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &input_handlers_seq_ops);
}