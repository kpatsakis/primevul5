static int io_setup_async_rw(struct io_kiocb *req, const struct iovec *iovec,
			     const struct iovec *fast_iov,
			     struct iov_iter *iter, bool force)
{
	if (!force && !io_op_defs[req->opcode].needs_async_setup)
		return 0;
	if (!req->async_data) {
		if (io_alloc_async_data(req)) {
			kfree(iovec);
			return -ENOMEM;
		}

		io_req_map_rw(req, iovec, fast_iov, iter);
	}
	return 0;
}