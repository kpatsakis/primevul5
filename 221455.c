static bool io_poll_remove_waitqs(struct io_kiocb *req)
	__must_hold(&req->ctx->completion_lock)
{
	bool do_complete;

	io_poll_remove_double(req);
	do_complete = __io_poll_remove_one(req, io_poll_get_single(req), true);

	if (req->opcode != IORING_OP_POLL_ADD && do_complete) {
		/* non-poll requests have submit ref still */
		req_ref_put(req);
	}
	return do_complete;