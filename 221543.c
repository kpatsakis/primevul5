
static int __io_sqe_buffers_update(struct io_ring_ctx *ctx,
				   struct io_uring_rsrc_update2 *up,
				   unsigned int nr_args)
{
	u64 __user *tags = u64_to_user_ptr(up->tags);
	struct iovec iov, __user *iovs = u64_to_user_ptr(up->data);
	struct page *last_hpage = NULL;
	bool needs_switch = false;
	__u32 done;
	int i, err;

	if (!ctx->buf_data)
		return -ENXIO;
	if (up->offset + nr_args > ctx->nr_user_bufs)
		return -EINVAL;

	for (done = 0; done < nr_args; done++) {
		struct io_mapped_ubuf *imu;
		int offset = up->offset + done;
		u64 tag = 0;

		err = io_copy_iov(ctx, &iov, iovs, done);
		if (err)
			break;
		if (tags && copy_from_user(&tag, &tags[done], sizeof(tag))) {
			err = -EFAULT;
			break;
		}
		err = io_buffer_validate(&iov);
		if (err)
			break;
		if (!iov.iov_base && tag) {
			err = -EINVAL;
			break;
		}
		err = io_sqe_buffer_register(ctx, &iov, &imu, &last_hpage);
		if (err)
			break;

		i = array_index_nospec(offset, ctx->nr_user_bufs);
		if (ctx->user_bufs[i] != ctx->dummy_ubuf) {
			err = io_queue_rsrc_removal(ctx->buf_data, offset,
						    ctx->rsrc_node, ctx->user_bufs[i]);
			if (unlikely(err)) {
				io_buffer_unmap(ctx, &imu);
				break;
			}
			ctx->user_bufs[i] = NULL;
			needs_switch = true;
		}

		ctx->user_bufs[i] = imu;
		ctx->buf_data->tags[offset] = tag;
	}

	if (needs_switch)
		io_rsrc_node_switch(ctx, ctx->buf_data);
	return done ? done : err;