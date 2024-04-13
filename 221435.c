static bool io_poll_complete(struct io_kiocb *req, __poll_t mask)
	__must_hold(&req->ctx->completion_lock)
{
	struct io_ring_ctx *ctx = req->ctx;
	unsigned flags = IORING_CQE_F_MORE;
	int error;

	if (READ_ONCE(req->poll.canceled)) {
		error = -ECANCELED;
		req->poll.events |= EPOLLONESHOT;
	} else {
		error = mangle_poll(mask);
	}
	if (req->poll.events & EPOLLONESHOT)
		flags = 0;
	if (!io_cqring_fill_event(ctx, req->user_data, error, flags)) {
		io_poll_remove_waitqs(req);
		req->poll.done = true;
		flags = 0;
	}
	if (flags & IORING_CQE_F_MORE)
		ctx->cq_extra++;

	io_commit_cqring(ctx);
	return !(flags & IORING_CQE_F_MORE);