static bool io_rw_should_retry(struct io_kiocb *req)
{
	struct kiocb *kiocb = &req->rw.kiocb;
	int ret;

	/* never retry for NOWAIT, we just complete with -EAGAIN */
	if (req->flags & REQ_F_NOWAIT)
		return false;

	/* already tried, or we're doing O_DIRECT */
	if (kiocb->ki_flags & (IOCB_DIRECT | IOCB_WAITQ))
		return false;
	/*
	 * just use poll if we can, and don't attempt if the fs doesn't
	 * support callback based unlocks
	 */
	if (file_can_poll(req->file) || !(req->file->f_mode & FMODE_BUF_RASYNC))
		return false;

	/*
	 * If request type doesn't require req->io to defer in general,
	 * we need to allocate it here
	 */
	if (!req->io && __io_alloc_async_ctx(req))
		return false;

	ret = kiocb_wait_page_queue_init(kiocb, &req->io->rw.wpq,
						io_async_buf_func, req);
	if (!ret) {
		io_get_req_task(req);
		return true;
	}

	return false;
}