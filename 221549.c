
static void __io_queue_sqe(struct io_kiocb *req)
{
	struct io_kiocb *linked_timeout = io_prep_linked_timeout(req);
	int ret;

	ret = io_issue_sqe(req, IO_URING_F_NONBLOCK|IO_URING_F_COMPLETE_DEFER);

	/*
	 * We async punt it if the file wasn't marked NOWAIT, or if the file
	 * doesn't support non-blocking read/write attempts
	 */
	if (likely(!ret)) {
		/* drop submission reference */
		if (req->flags & REQ_F_COMPLETE_INLINE) {
			struct io_ring_ctx *ctx = req->ctx;
			struct io_comp_state *cs = &ctx->submit_state.comp;

			cs->reqs[cs->nr++] = req;
			if (cs->nr == ARRAY_SIZE(cs->reqs))
				io_submit_flush_completions(cs, ctx);
		} else {
			io_put_req(req);
		}
	} else if (ret == -EAGAIN && !(req->flags & REQ_F_NOWAIT)) {
		if (!io_arm_poll_handler(req)) {
			/*
			 * Queued up for async execution, worker will release
			 * submit reference when the iocb is actually submitted.
			 */
			io_queue_async_work(req);
		}
	} else {
		io_req_complete_failed(req, ret);
	}
	if (linked_timeout)
		io_queue_linked_timeout(linked_timeout);