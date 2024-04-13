static int io_statx(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_statx *ctx = &req->statx;
	int ret;

	if (issue_flags & IO_URING_F_NONBLOCK)
		return -EAGAIN;

	ret = do_statx(ctx->dfd, ctx->filename, ctx->flags, ctx->mask,
		       ctx->buffer);

	if (ret < 0)
		req_set_fail_links(req);
	io_req_complete(req, ret);
	return 0;
}