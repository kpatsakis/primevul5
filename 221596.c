static void io_iopoll_complete(struct io_ring_ctx *ctx, unsigned int *nr_events,
			       struct list_head *done)
{
	struct req_batch rb;
	struct io_kiocb *req;

	/* order with ->result store in io_complete_rw_iopoll() */
	smp_rmb();

	io_init_req_batch(&rb);
	while (!list_empty(done)) {
		int cflags = 0;

		req = list_first_entry(done, struct io_kiocb, inflight_entry);
		list_del(&req->inflight_entry);

		if (READ_ONCE(req->result) == -EAGAIN &&
		    !(req->flags & REQ_F_DONT_REISSUE)) {
			req->iopoll_completed = 0;
			req_ref_get(req);
			io_queue_async_work(req);
			continue;
		}

		if (req->flags & REQ_F_BUFFER_SELECTED)
			cflags = io_put_rw_kbuf(req);

		__io_cqring_fill_event(ctx, req->user_data, req->result, cflags);
		(*nr_events)++;

		if (req_ref_put_and_test(req))
			io_req_free_batch(&rb, req, &ctx->submit_state);
	}

	io_commit_cqring(ctx);
	io_cqring_ev_posted_iopoll(ctx);
	io_req_free_batch_finish(ctx, &rb);
}