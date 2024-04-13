static inline int io_rw_prep_async(struct io_kiocb *req, int rw,
				   bool force_nonblock)
{
	struct io_async_ctx *io = req->io;
	struct iov_iter iter;
	ssize_t ret;

	io->rw.iov = io->rw.fast_iov;
	req->io = NULL;
	ret = io_import_iovec(rw, req, &io->rw.iov, &iter, !force_nonblock);
	req->io = io;
	if (unlikely(ret < 0))
		return ret;

	io_req_map_rw(req, ret, io->rw.iov, io->rw.fast_iov, &iter);
	return 0;
}