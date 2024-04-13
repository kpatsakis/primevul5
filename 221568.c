static int io_prep_rw(struct io_kiocb *req, const struct io_uring_sqe *sqe)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct kiocb *kiocb = &req->rw.kiocb;
	struct file *file = req->file;
	unsigned ioprio;
	int ret;

	if (!(req->flags & REQ_F_ISREG) && S_ISREG(file_inode(file)->i_mode))
		req->flags |= REQ_F_ISREG;

	kiocb->ki_pos = READ_ONCE(sqe->off);
	if (kiocb->ki_pos == -1 && !(file->f_mode & FMODE_STREAM)) {
		req->flags |= REQ_F_CUR_POS;
		kiocb->ki_pos = file->f_pos;
	}
	kiocb->ki_hint = ki_hint_validate(file_write_hint(kiocb->ki_filp));
	kiocb->ki_flags = iocb_flags(kiocb->ki_filp);
	ret = kiocb_set_rw_flags(kiocb, READ_ONCE(sqe->rw_flags));
	if (unlikely(ret))
		return ret;

	/* don't allow async punt for O_NONBLOCK or RWF_NOWAIT */
	if ((kiocb->ki_flags & IOCB_NOWAIT) || (file->f_flags & O_NONBLOCK))
		req->flags |= REQ_F_NOWAIT;

	ioprio = READ_ONCE(sqe->ioprio);
	if (ioprio) {
		ret = ioprio_check_cap(ioprio);
		if (ret)
			return ret;

		kiocb->ki_ioprio = ioprio;
	} else
		kiocb->ki_ioprio = get_current_ioprio();

	if (ctx->flags & IORING_SETUP_IOPOLL) {
		if (!(kiocb->ki_flags & IOCB_DIRECT) ||
		    !kiocb->ki_filp->f_op->iopoll)
			return -EOPNOTSUPP;

		kiocb->ki_flags |= IOCB_HIPRI;
		kiocb->ki_complete = io_complete_rw_iopoll;
		req->iopoll_completed = 0;
	} else {
		if (kiocb->ki_flags & IOCB_HIPRI)
			return -EINVAL;
		kiocb->ki_complete = io_complete_rw;
	}

	if (req->opcode == IORING_OP_READ_FIXED ||
	    req->opcode == IORING_OP_WRITE_FIXED) {
		req->imu = NULL;
		io_req_set_rsrc_node(req);
	}

	req->rw.addr = READ_ONCE(sqe->addr);
	req->rw.len = READ_ONCE(sqe->len);
	req->buf_index = READ_ONCE(sqe->buf_index);
	return 0;
}