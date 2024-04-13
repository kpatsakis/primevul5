
static int io_register_rsrc_update(struct io_ring_ctx *ctx, void __user *arg,
				   unsigned size)
{
	struct io_uring_rsrc_update2 up;

	if (size != sizeof(up))
		return -EINVAL;
	if (copy_from_user(&up, arg, sizeof(up)))
		return -EFAULT;
	if (!up.nr)
		return -EINVAL;
	return __io_register_rsrc_update(ctx, up.type, &up, up.nr);