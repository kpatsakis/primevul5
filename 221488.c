
static int io_files_update(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct io_uring_rsrc_update2 up;
	int ret;

	if (issue_flags & IO_URING_F_NONBLOCK)
		return -EAGAIN;

	up.offset = req->rsrc_update.offset;
	up.data = req->rsrc_update.arg;
	up.nr = 0;
	up.tags = 0;
	up.resv = 0;

	mutex_lock(&ctx->uring_lock);
	ret = __io_register_rsrc_update(ctx, IORING_RSRC_FILE,
					&up, req->rsrc_update.nr_args);
	mutex_unlock(&ctx->uring_lock);

	if (ret < 0)
		req_set_fail_links(req);
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;