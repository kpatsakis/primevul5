
static int io_init_req(struct io_ring_ctx *ctx, struct io_kiocb *req,
		       const struct io_uring_sqe *sqe)
{
	struct io_submit_state *state;
	unsigned int sqe_flags;
	int personality, ret = 0;

	req->opcode = READ_ONCE(sqe->opcode);
	/* same numerical values with corresponding REQ_F_*, safe to copy */
	req->flags = sqe_flags = READ_ONCE(sqe->flags);
	req->user_data = READ_ONCE(sqe->user_data);
	req->async_data = NULL;
	req->file = NULL;
	req->ctx = ctx;
	req->link = NULL;
	req->fixed_rsrc_refs = NULL;
	/* one is dropped after submission, the other at completion */
	atomic_set(&req->refs, 2);
	req->task = current;
	req->result = 0;
	req->work.creds = NULL;

	/* enforce forwards compatibility on users */
	if (unlikely(sqe_flags & ~SQE_VALID_FLAGS))
		return -EINVAL;
	if (unlikely(req->opcode >= IORING_OP_LAST))
		return -EINVAL;
	if (unlikely(!io_check_restriction(ctx, req, sqe_flags)))
		return -EACCES;

	if ((sqe_flags & IOSQE_BUFFER_SELECT) &&
	    !io_op_defs[req->opcode].buffer_select)
		return -EOPNOTSUPP;

	personality = READ_ONCE(sqe->personality);
	if (personality) {
		req->work.creds = xa_load(&ctx->personalities, personality);
		if (!req->work.creds)
			return -EINVAL;
		get_cred(req->work.creds);
	}
	state = &ctx->submit_state;

	/*
	 * Plug now if we have more than 1 IO left after this, and the target
	 * is potentially a read/write to block based storage.
	 */
	if (!state->plug_started && state->ios_left > 1 &&
	    io_op_defs[req->opcode].plug) {
		blk_start_plug(&state->plug);
		state->plug_started = true;
	}

	if (io_op_defs[req->opcode].needs_file) {
		bool fixed = req->flags & REQ_F_FIXED_FILE;

		req->file = io_file_get(state, req, READ_ONCE(sqe->fd), fixed);
		if (unlikely(!req->file))
			ret = -EBADF;
	}

	state->ios_left--;
	return ret;