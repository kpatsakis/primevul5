static bool io_rw_should_reissue(struct io_kiocb *req)
{
	umode_t mode = file_inode(req->file)->i_mode;
	struct io_ring_ctx *ctx = req->ctx;

	if (!S_ISBLK(mode) && !S_ISREG(mode))
		return false;
	if ((req->flags & REQ_F_NOWAIT) || (io_wq_current_is_worker() &&
	    !(ctx->flags & IORING_SETUP_IOPOLL)))
		return false;
	/*
	 * If ref is dying, we might be running poll reap from the exit work.
	 * Don't attempt to reissue from that path, just let it fail with
	 * -EAGAIN.
	 */
	if (percpu_ref_is_dying(&ctx->refs))
		return false;
	return true;
}