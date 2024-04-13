int kernel_read_file_from_path(const char *path, void **buf, loff_t *size,
			       loff_t max_size, enum kernel_read_file_id id)
{
	struct file *file;
	int ret;

	if (!path || !*path)
		return -EINVAL;

	file = filp_open(path, O_RDONLY, 0);
	if (IS_ERR(file))
		return PTR_ERR(file);

	ret = kernel_read_file(file, buf, size, max_size, id);
	fput(file);
	return ret;
}