static void kiocb_done(struct kiocb *kiocb, ssize_t ret,
		       struct io_comp_state *cs)
{
	struct io_kiocb *req = container_of(kiocb, struct io_kiocb, rw.kiocb);

	if (req->flags & REQ_F_CUR_POS)
		req->file->f_pos = kiocb->ki_pos;
	if (ret >= 0 && kiocb->ki_complete == io_complete_rw)
		__io_complete_rw(req, ret, 0, cs);
	else
		io_rw_done(kiocb, ret);
}