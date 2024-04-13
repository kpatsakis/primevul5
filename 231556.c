loff_t tracing_lseek(struct file *file, loff_t offset, int whence)
{
	int ret;

	if (file->f_mode & FMODE_READ)
		ret = seq_lseek(file, offset, whence);
	else
		file->f_pos = ret = 0;

	return ret;
}