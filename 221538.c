static void __io_complete_rw(struct io_kiocb *req, long res, long res2,
			     unsigned int issue_flags)
{
	int cflags = 0;

	if (req->rw.kiocb.ki_flags & IOCB_WRITE)
		kiocb_end_write(req);
	if (res != req->result) {
		if ((res == -EAGAIN || res == -EOPNOTSUPP) &&
		    io_rw_should_reissue(req)) {
			req->flags |= REQ_F_REISSUE;
			return;
		}
		req_set_fail_links(req);
	}
	if (req->flags & REQ_F_BUFFER_SELECTED)
		cflags = io_put_rw_kbuf(req);
	__io_req_complete(req, issue_flags, res, cflags);
}