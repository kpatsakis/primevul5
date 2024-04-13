
static int __io_register_rsrc_update(struct io_ring_ctx *ctx, unsigned type,
				     struct io_uring_rsrc_update2 *up,
				     unsigned nr_args)
{
	__u32 tmp;
	int err;

	if (up->resv)
		return -EINVAL;
	if (check_add_overflow(up->offset, nr_args, &tmp))
		return -EOVERFLOW;
	err = io_rsrc_node_switch_start(ctx);
	if (err)
		return err;

	switch (type) {
	case IORING_RSRC_FILE:
		return __io_sqe_files_update(ctx, up, nr_args);
	case IORING_RSRC_BUFFER:
		return __io_sqe_buffers_update(ctx, up, nr_args);
	}
	return -EINVAL;