static void kiocb_done(struct kiocb *kiocb, ssize_t ret,
		       unsigned int issue_flags)
{
	struct io_kiocb *req = container_of(kiocb, struct io_kiocb, rw.kiocb);
	struct io_async_rw *io = req->async_data;
	bool check_reissue = kiocb->ki_complete == io_complete_rw;

	/* add previously done IO, if any */
	if (io && io->bytes_done > 0) {
		if (ret < 0)
			ret = io->bytes_done;
		else
			ret += io->bytes_done;
	}

	if (req->flags & REQ_F_CUR_POS)
		req->file->f_pos = kiocb->ki_pos;
	if (ret >= 0 && kiocb->ki_complete == io_complete_rw)
		__io_complete_rw(req, ret, 0, issue_flags);
	else
		io_rw_done(kiocb, ret);

	if (check_reissue && req->flags & REQ_F_REISSUE) {
		req->flags &= ~REQ_F_REISSUE;
		if (io_resubmit_prep(req)) {
			req_ref_get(req);
			io_queue_async_work(req);
		} else {
			int cflags = 0;

			req_set_fail_links(req);
			if (req->flags & REQ_F_BUFFER_SELECTED)
				cflags = io_put_rw_kbuf(req);
			__io_req_complete(req, issue_flags, ret, cflags);
		}
	}
}