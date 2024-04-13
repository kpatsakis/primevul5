static int io_recvmsg_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct io_sr_msg *sr = &req->sr_msg;

	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;

	sr->umsg = u64_to_user_ptr(READ_ONCE(sqe->addr));
	sr->len = READ_ONCE(sqe->len);
	sr->bgid = READ_ONCE(sqe->buf_group);
	sr->msg_flags = READ_ONCE(sqe->msg_flags) | MSG_NOSIGNAL;
	if (sr->msg_flags & MSG_DONTWAIT)
		req->flags |= REQ_F_NOWAIT;

#ifdef CONFIG_COMPAT
	if (req->ctx->compat)
		sr->msg_flags |= MSG_CMSG_COMPAT;
#endif
	return 0;
}