static int __io_splice_prep(struct io_kiocb *req,
			    const struct io_uring_sqe *sqe)
{
	struct io_splice* sp = &req->splice;
	unsigned int valid_flags = SPLICE_F_FD_IN_FIXED | SPLICE_F_ALL;

	if (unlikely(req->ctx->flags & IORING_SETUP_IOPOLL))
		return -EINVAL;

	sp->file_in = NULL;
	sp->len = READ_ONCE(sqe->len);
	sp->flags = READ_ONCE(sqe->splice_flags);

	if (unlikely(sp->flags & ~valid_flags))
		return -EINVAL;

	sp->file_in = io_file_get(NULL, req, READ_ONCE(sqe->splice_fd_in),
				  (sp->flags & SPLICE_F_FD_IN_FIXED));
	if (!sp->file_in)
		return -EBADF;
	req->flags |= REQ_F_NEED_CLEANUP;
	return 0;
}