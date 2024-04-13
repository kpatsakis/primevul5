static int input_proc_devices_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &input_devices_seq_ops);
}