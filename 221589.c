
static int io_sqe_buffers_register(struct io_ring_ctx *ctx, void __user *arg,
				   unsigned int nr_args, u64 __user *tags)
{
	struct page *last_hpage = NULL;
	struct io_rsrc_data *data;
	int i, ret;
	struct iovec iov;

	if (ctx->user_bufs)
		return -EBUSY;
	if (!nr_args || nr_args > UIO_MAXIOV)
		return -EINVAL;
	ret = io_rsrc_node_switch_start(ctx);
	if (ret)
		return ret;
	data = io_rsrc_data_alloc(ctx, io_rsrc_buf_put, nr_args);
	if (!data)
		return -ENOMEM;
	ret = io_buffers_map_alloc(ctx, nr_args);
	if (ret) {
		io_rsrc_data_free(data);
		return ret;
	}

	for (i = 0; i < nr_args; i++, ctx->nr_user_bufs++) {
		u64 tag = 0;

		if (tags && copy_from_user(&tag, &tags[i], sizeof(tag))) {
			ret = -EFAULT;
			break;
		}
		ret = io_copy_iov(ctx, &iov, arg, i);
		if (ret)
			break;
		ret = io_buffer_validate(&iov);
		if (ret)
			break;
		if (!iov.iov_base && tag) {
			ret = -EINVAL;
			break;
		}

		ret = io_sqe_buffer_register(ctx, &iov, &ctx->user_bufs[i],
					     &last_hpage);
		if (ret)
			break;
		data->tags[i] = tag;
	}

	WARN_ON_ONCE(ctx->buf_data);

	ctx->buf_data = data;
	if (ret)
		__io_sqe_buffers_unregister(ctx);
	else
		io_rsrc_node_switch(ctx, NULL);
	return ret;