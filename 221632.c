static inline void io_put_file(struct file *file)
{
	if (file)
		fput(file);
}