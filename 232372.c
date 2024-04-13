static int io_openat_prep(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	u64 flags, mode;

	if (req->flags & REQ_F_NEED_CLEANUP)
		return 0;
	mode = READ_ONCE(sqe->len);
	flags = READ_ONCE(sqe->open_flags);
	req->open.how = build_open_how(flags, mode);
	return __io_openat_prep(req, sqe);
}