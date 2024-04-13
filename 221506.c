static void io_prep_async_work(struct io_kiocb *req)
{
	const struct io_op_def *def = &io_op_defs[req->opcode];
	struct io_ring_ctx *ctx = req->ctx;

	if (!req->work.creds)
		req->work.creds = get_current_cred();

	req->work.list.next = NULL;
	req->work.flags = 0;
	if (req->flags & REQ_F_FORCE_ASYNC)
		req->work.flags |= IO_WQ_WORK_CONCURRENT;

	if (req->flags & REQ_F_ISREG) {
		if (def->hash_reg_file || (ctx->flags & IORING_SETUP_IOPOLL))
			io_wq_hash_work(&req->work, file_inode(req->file));
	} else if (!req->file || !S_ISBLK(file_inode(req->file)->i_mode)) {
		if (def->unbound_nonreg_file)
			req->work.flags |= IO_WQ_WORK_UNBOUND;
	}

	switch (req->opcode) {
	case IORING_OP_SPLICE:
	case IORING_OP_TEE:
		if (!S_ISREG(file_inode(req->splice.file_in)->i_mode))
			req->work.flags |= IO_WQ_WORK_UNBOUND;
		break;
	}
}