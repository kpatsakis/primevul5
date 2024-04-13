static int io_write(struct io_kiocb *req, bool force_nonblock,
		    struct io_comp_state *cs)
{
	struct iovec inline_vecs[UIO_FASTIOV], *iovec = inline_vecs;
	struct kiocb *kiocb = &req->rw.kiocb;
	struct iov_iter iter;
	size_t iov_count;
	ssize_t ret, ret2, io_size;
	unsigned long nr_segs;

	ret = io_import_iovec(WRITE, req, &iovec, &iter, !force_nonblock);
	if (ret < 0)
		return ret;
	io_size = ret;
	req->result = io_size;

	/* Ensure we clear previously set non-block flag */
	if (!force_nonblock)
		req->rw.kiocb.ki_flags &= ~IOCB_NOWAIT;

	/* If the file doesn't support async, just async punt */
	if (force_nonblock && !io_file_supports_async(req->file, WRITE))
		goto copy_iov;

	/* file path doesn't support NOWAIT for non-direct_IO */
	if (force_nonblock && !(kiocb->ki_flags & IOCB_DIRECT) &&
	    (req->flags & REQ_F_ISREG))
		goto copy_iov;

	iov_count = iov_iter_count(&iter);
	nr_segs = iter.nr_segs;
	ret = rw_verify_area(WRITE, req->file, &kiocb->ki_pos, iov_count);
	if (unlikely(ret))
		goto out_free;

	/*
	 * Open-code file_start_write here to grab freeze protection,
	 * which will be released by another thread in
	 * io_complete_rw().  Fool lockdep by telling it the lock got
	 * released so that it doesn't complain about the held lock when
	 * we return to userspace.
	 */
	if (req->flags & REQ_F_ISREG) {
		__sb_start_write(file_inode(req->file)->i_sb,
					SB_FREEZE_WRITE, true);
		__sb_writers_release(file_inode(req->file)->i_sb,
					SB_FREEZE_WRITE);
	}
	kiocb->ki_flags |= IOCB_WRITE;

	if (req->file->f_op->write_iter)
		ret2 = call_write_iter(req->file, kiocb, &iter);
	else if (req->file->f_op->write)
		ret2 = loop_rw_iter(WRITE, req->file, kiocb, &iter);
	else
		ret2 = -EINVAL;

	/*
	 * Raw bdev writes will return -EOPNOTSUPP for IOCB_NOWAIT. Just
	 * retry them without IOCB_NOWAIT.
	 */
	if (ret2 == -EOPNOTSUPP && (kiocb->ki_flags & IOCB_NOWAIT))
		ret2 = -EAGAIN;
	if (!force_nonblock || ret2 != -EAGAIN) {
		kiocb_done(kiocb, ret2, cs);
	} else {
		iter.count = iov_count;
		iter.nr_segs = nr_segs;
copy_iov:
		ret = io_setup_async_rw(req, io_size, iovec, inline_vecs,
					&iter);
		if (ret)
			goto out_free;
		/* it's copied and will be cleaned with ->io */
		iovec = NULL;
		return -EAGAIN;
	}
out_free:
	if (iovec)
		kfree(iovec);
	return ret;
}