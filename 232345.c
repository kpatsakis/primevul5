static void io_req_map_rw(struct io_kiocb *req, ssize_t io_size,
			  struct iovec *iovec, struct iovec *fast_iov,
			  struct iov_iter *iter)
{
	struct io_async_rw *rw = &req->io->rw;

	rw->nr_segs = iter->nr_segs;
	rw->size = io_size;
	if (!iovec) {
		rw->iov = rw->fast_iov;
		if (rw->iov != fast_iov)
			memcpy(rw->iov, fast_iov,
			       sizeof(struct iovec) * iter->nr_segs);
	} else {
		rw->iov = iovec;
		req->flags |= REQ_F_NEED_CLEANUP;
	}
}