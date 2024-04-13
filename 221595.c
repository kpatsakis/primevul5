
static int io_poll_add_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct io_poll_iocb *poll = &req->poll;
	u32 flags;

	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;
	if (sqe->ioprio || sqe->buf_index || sqe->off || sqe->addr)
		return -EINVAL;
	flags = READ_ONCE(sqe->len);
	if (flags & ~IORING_POLL_ADD_MULTI)
		return -EINVAL;

	poll->events = io_poll_parse_events(sqe, flags);
	return 0;