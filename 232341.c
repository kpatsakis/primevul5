static int io_setup_async_rw(struct io_kiocb *req, ssize_t io_size,
			     struct iovec *iovec, struct iovec *fast_iov,
			     struct iov_iter *iter)
{
	if (!io_op_defs[req->opcode].async_ctx)
		return 0;
	if (!req->io) {
		if (__io_alloc_async_ctx(req))
			return -ENOMEM;

		io_req_map_rw(req, io_size, iovec, fast_iov, iter);
	}
	return 0;
}