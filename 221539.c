static int io_connect(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_async_connect __io, *io;
	unsigned file_flags;
	int ret;
	bool force_nonblock = issue_flags & IO_URING_F_NONBLOCK;

	if (req->async_data) {
		io = req->async_data;
	} else {
		ret = move_addr_to_kernel(req->connect.addr,
						req->connect.addr_len,
						&__io.address);
		if (ret)
			goto out;
		io = &__io;
	}

	file_flags = force_nonblock ? O_NONBLOCK : 0;

	ret = __sys_connect_file(req->file, &io->address,
					req->connect.addr_len, file_flags);
	if ((ret == -EAGAIN || ret == -EINPROGRESS) && force_nonblock) {
		if (req->async_data)
			return -EAGAIN;
		if (io_alloc_async_data(req)) {
			ret = -ENOMEM;
			goto out;
		}
		memcpy(req->async_data, &__io, sizeof(__io));
		return -EAGAIN;
	}
	if (ret == -ERESTARTSYS)
		ret = -EINTR;
out:
	if (ret < 0)
		req_set_fail_links(req);
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;
}