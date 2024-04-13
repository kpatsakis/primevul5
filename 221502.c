
static int io_uring_install_fd(struct io_ring_ctx *ctx, struct file *file)
{
	int ret, fd;

	fd = get_unused_fd_flags(O_RDWR | O_CLOEXEC);
	if (fd < 0)
		return fd;

	ret = io_uring_add_task_file(ctx);
	if (ret) {
		put_unused_fd(fd);
		return ret;
	}
	fd_install(fd, file);
	return fd;