static int io_read(struct io_kiocb *req, bool force_nonblock,
		   struct io_comp_state *cs)
{
	struct iovec inline_vecs[UIO_FASTIOV], *iovec = inline_vecs;
	struct kiocb *kiocb = &req->rw.kiocb;
	struct iov_iter iter;
	size_t iov_count;
	ssize_t io_size, ret, ret2;
	unsigned long nr_segs;

	ret = io_import_iovec(READ, req, &iovec, &iter, !force_nonblock);
	if (ret < 0)
		return ret;
	io_size = ret;
	req->result = io_size;

	/* Ensure we clear previously set non-block flag */
	if (!force_nonblock)
		kiocb->ki_flags &= ~IOCB_NOWAIT;

	/* If the file doesn't support async, just async punt */
	if (force_nonblock && !io_file_supports_async(req->file, READ))
		goto copy_iov;

	iov_count = iov_iter_count(&iter);
	nr_segs = iter.nr_segs;
	ret = rw_verify_area(READ, req->file, &kiocb->ki_pos, iov_count);
	if (unlikely(ret))
		goto out_free;

	ret2 = io_iter_do_read(req, &iter);

	/* Catch -EAGAIN return for forced non-blocking submission */
	if (!force_nonblock || (ret2 != -EAGAIN && ret2 != -EIO)) {
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
		/* if we can retry, do so with the callbacks armed */
		if (io_rw_should_retry(req)) {
			ret2 = io_iter_do_read(req, &iter);
			if (ret2 == -EIOCBQUEUED) {
				goto out_free;
			} else if (ret2 != -EAGAIN) {
				kiocb_done(kiocb, ret2, cs);
				goto out_free;
			}
		}
		kiocb->ki_flags &= ~IOCB_WAITQ;
		return -EAGAIN;
	}
out_free:
	if (iovec)
		kfree(iovec);
	return ret;
}