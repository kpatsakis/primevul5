
static int io_register_files_update(struct io_ring_ctx *ctx, void __user *arg,
				    unsigned nr_args)
{
	struct io_uring_rsrc_update2 up;

	if (!nr_args)
		return -EINVAL;
	memset(&up, 0, sizeof(up));
	if (copy_from_user(&up, arg, sizeof(struct io_uring_rsrc_update)))
		return -EFAULT;
	return __io_register_rsrc_update(ctx, IORING_RSRC_FILE, &up, nr_args);