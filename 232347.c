static int __io_openat_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	const char __user *fname;
	int ret;

	if (unlikely(req->ctx->flags & (IORING_SETUP_IOPOLL|IORING_SETUP_SQPOLL)))
		return -EINVAL;
	if (unlikely(sqe->ioprio || sqe->buf_index))
		return -EINVAL;
	if (unlikely(req->flags & REQ_F_FIXED_FILE))
		return -EBADF;

	/* open.how should be already initialised */
	if (!(req->open.how.flags & O_PATH) && force_o_largefile())
		req->open.how.flags |= O_LARGEFILE;

	req->open.dfd = READ_ONCE(sqe->fd);
	fname = u64_to_user_ptr(READ_ONCE(sqe->addr));
	req->open.filename = getname(fname);
	if (IS_ERR(req->open.filename)) {
		ret = PTR_ERR(req->open.filename);
		req->open.filename = NULL;
		return ret;
	}
	req->open.nofile = rlimit(RLIMIT_NOFILE);
	req->flags |= REQ_F_NEED_CLEANUP;
	return 0;
}