
static void io_poll_task_func(struct callback_head *cb)
{
	struct io_kiocb *req = container_of(cb, struct io_kiocb, task_work);
	struct io_ring_ctx *ctx = req->ctx;
	struct io_kiocb *nxt;

	if (io_poll_rewait(req, &req->poll)) {
		spin_unlock_irq(&ctx->completion_lock);
	} else {
		bool done;

		done = io_poll_complete(req, req->result);
		if (done) {
			hash_del(&req->hash_node);
		} else {
			req->result = 0;
			add_wait_queue(req->poll.head, &req->poll.wait);
		}
		spin_unlock_irq(&ctx->completion_lock);
		io_cqring_ev_posted(ctx);

		if (done) {
			nxt = io_put_req_find_next(req);
			if (nxt)
				__io_req_task_submit(nxt);
		}
	}