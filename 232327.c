static bool io_resubmit_prep(struct io_kiocb *req, int error)
{
	struct iovec inline_vecs[UIO_FASTIOV], *iovec = inline_vecs;
	ssize_t ret = -ECANCELED;
	struct iov_iter iter;
	int rw;

	if (error) {
		ret = error;
		goto end_req;
	}

	switch (req->opcode) {
	case IORING_OP_READV:
	case IORING_OP_READ_FIXED:
	case IORING_OP_READ:
		rw = READ;
		break;
	case IORING_OP_WRITEV:
	case IORING_OP_WRITE_FIXED:
	case IORING_OP_WRITE:
		rw = WRITE;
		break;
	default:
		printk_once(KERN_WARNING "io_uring: bad opcode in resubmit %d\n",
				req->opcode);
		goto end_req;
	}

	ret = io_import_iovec(rw, req, &iovec, &iter, false);
	if (ret < 0)
		goto end_req;
	ret = io_setup_async_rw(req, ret, iovec, inline_vecs, &iter);
	if (!ret)
		return true;
	kfree(iovec);
end_req:
	req_set_fail_links(req);
	io_req_complete(req, ret);
	return false;
}