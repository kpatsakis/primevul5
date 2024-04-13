static ssize_t io_import_iovec(int rw, struct io_kiocb *req,
			       struct iovec **iovec, struct iov_iter *iter,
			       bool needs_lock)
{
	void __user *buf = u64_to_user_ptr(req->rw.addr);
	size_t sqe_len = req->rw.len;
	ssize_t ret;
	u8 opcode;

	opcode = req->opcode;
	if (opcode == IORING_OP_READ_FIXED || opcode == IORING_OP_WRITE_FIXED) {
		*iovec = NULL;
		return io_import_fixed(req, rw, iter);
	}

	/* buffer index only valid with fixed read/write, or buffer select  */
	if (req->buf_index && !(req->flags & REQ_F_BUFFER_SELECT))
		return -EINVAL;

	if (opcode == IORING_OP_READ || opcode == IORING_OP_WRITE) {
		if (req->flags & REQ_F_BUFFER_SELECT) {
			buf = io_rw_buffer_select(req, &sqe_len, needs_lock);
			if (IS_ERR(buf)) {
				*iovec = NULL;
				return PTR_ERR(buf);
			}
			req->rw.len = sqe_len;
		}

		ret = import_single_range(rw, buf, sqe_len, *iovec, iter);
		*iovec = NULL;
		return ret < 0 ? ret : sqe_len;
	}

	if (req->io) {
		struct io_async_rw *iorw = &req->io->rw;

		iov_iter_init(iter, rw, iorw->iov, iorw->nr_segs, iorw->size);
		*iovec = NULL;
		return iorw->size;
	}

	if (req->flags & REQ_F_BUFFER_SELECT) {
		ret = io_iov_buffer_select(req, *iovec, needs_lock);
		if (!ret) {
			ret = (*iovec)->iov_len;
			iov_iter_init(iter, rw, *iovec, 1, ret);
		}
		*iovec = NULL;
		return ret;
	}

#ifdef CONFIG_COMPAT
	if (req->ctx->compat)
		return compat_import_iovec(rw, buf, sqe_len, UIO_FASTIOV,
						iovec, iter);
#endif

	return import_iovec(rw, buf, sqe_len, UIO_FASTIOV, iovec, iter);
}