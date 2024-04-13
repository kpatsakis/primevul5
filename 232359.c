static void io_iopoll_complete(struct io_ring_ctx *ctx, unsigned int *nr_events,
			       struct list_head *done)
{
	struct req_batch rb;
	struct io_kiocb *req;
	LIST_HEAD(again);

	/* order with ->result store in io_complete_rw_iopoll() */
	smp_rmb();

	io_init_req_batch(&rb);
	while (!list_empty(done)) {
		int cflags = 0;

		req = list_first_entry(done, struct io_kiocb, inflight_entry);
		if (READ_ONCE(req->result) == -EAGAIN) {
			req->iopoll_completed = 0;
			list_move_tail(&req->inflight_entry, &again);
			continue;
		}
		list_del(&req->inflight_entry);

		if (req->flags & REQ_F_BUFFER_SELECTED)
			cflags = io_put_rw_kbuf(req);

		__io_cqring_fill_event(req, req->result, cflags);
		(*nr_events)++;

		if (refcount_dec_and_test(&req->refs))
			io_req_free_batch(&rb, req);
	}

	io_commit_cqring(ctx);
	if (ctx->flags & IORING_SETUP_SQPOLL)
		io_cqring_ev_posted(ctx);
	io_req_free_batch_finish(ctx, &rb);

	if (!list_empty(&again))
		io_iopoll_queue(&again);
}