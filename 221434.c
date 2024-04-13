
static int io_timeout_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe,
			   bool is_timeout_link)
{
	struct io_timeout_data *data;
	unsigned flags;
	u32 off = READ_ONCE(sqe->off);

	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (sqe->ioprio || sqe->buf_index || sqe->len != 1)
		return -EINVAL;
	if (off && is_timeout_link)
		return -EINVAL;
	flags = READ_ONCE(sqe->timeout_flags);
	if (flags & ~IORING_TIMEOUT_ABS)
		return -EINVAL;

	req->timeout.off = off;

	if (!req->async_data && io_alloc_async_data(req))
		return -ENOMEM;

	data = req->async_data;
	data->req = req;

	if (get_timespec64(&data->ts, u64_to_user_ptr(sqe->addr)))
		return -EFAULT;

	data->mode = io_translate_timeout_mode(flags);
	hrtimer_init(&data->timer, CLOCK_MONOTONIC, data->mode);
	if (is_timeout_link)
		io_req_track_inflight(req);
	return 0;