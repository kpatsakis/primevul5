static int io_fadvise(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_fadvise *fa = &req->fadvise;
	int ret;

	if (issue_flags & IO_URING_F_NONBLOCK) {
		switch (fa->advice) {
		case POSIX_FADV_NORMAL:
		case POSIX_FADV_RANDOM:
		case POSIX_FADV_SEQUENTIAL:
			break;
		default:
			return -EAGAIN;
		}
	}

	ret = vfs_fadvise(req->file, fa->offset, fa->len, fa->advice);
	if (ret < 0)
		req_set_fail_links(req);
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;
}