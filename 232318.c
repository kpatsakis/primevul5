static int io_prep_rw(struct io_kiocb *req, const struct io_uring_sqe *sqe,
		      bool force_nonblock)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct kiocb *kiocb = &req->rw.kiocb;
	unsigned ioprio;
	int ret;

	if (S_ISREG(file_inode(req->file)->i_mode))
		req->flags |= REQ_F_ISREG;

	kiocb->ki_pos = READ_ONCE(sqe->off);
	if (kiocb->ki_pos == -1 && !(req->file->f_mode & FMODE_STREAM)) {
		req->flags |= REQ_F_CUR_POS;
		kiocb->ki_pos = req->file->f_pos;
	}
	kiocb->ki_hint = ki_hint_validate(file_write_hint(kiocb->ki_filp));
	kiocb->ki_flags = iocb_flags(kiocb->ki_filp);
	ret = kiocb_set_rw_flags(kiocb, READ_ONCE(sqe->rw_flags));
	if (unlikely(ret))
		return ret;

	ioprio = READ_ONCE(sqe->ioprio);
	if (ioprio) {
		ret = ioprio_check_cap(ioprio);
		if (ret)
			return ret;

		kiocb->ki_ioprio = ioprio;
	} else
		kiocb->ki_ioprio = get_current_ioprio();

	/* don't allow async punt if RWF_NOWAIT was requested */
	if (kiocb->ki_flags & IOCB_NOWAIT)
		req->flags |= REQ_F_NOWAIT;

	if (kiocb->ki_flags & IOCB_DIRECT)
		io_get_req_task(req);

	if (force_nonblock)
		kiocb->ki_flags |= IOCB_NOWAIT;

	if (ctx->flags & IORING_SETUP_IOPOLL) {
		if (!(kiocb->ki_flags & IOCB_DIRECT) ||
		    !kiocb->ki_filp->f_op->iopoll)
			return -EOPNOTSUPP;

		kiocb->ki_flags |= IOCB_HIPRI;
		kiocb->ki_complete = io_complete_rw_iopoll;
		req->iopoll_completed = 0;
		io_get_req_task(req);
	} else {
		if (kiocb->ki_flags & IOCB_HIPRI)
			return -EINVAL;
		kiocb->ki_complete = io_complete_rw;
	}

	req->rw.addr = READ_ONCE(sqe->addr);
	req->rw.len = READ_ONCE(sqe->len);
	req->buf_index = READ_ONCE(sqe->buf_index);
	return 0;
}