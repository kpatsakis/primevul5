
static int io_poll_add(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_poll_iocb *poll = &req->poll;
	struct io_ring_ctx *ctx = req->ctx;
	struct io_poll_table ipt;
	__poll_t mask;

	ipt.pt._qproc = io_poll_queue_proc;

	mask = __io_arm_poll_handler(req, &req->poll, &ipt, poll->events,
					io_poll_wake);

	if (mask) { /* no async, we'd stolen it */
		ipt.error = 0;
		io_poll_complete(req, mask);
	}
	spin_unlock_irq(&ctx->completion_lock);

	if (mask) {
		io_cqring_ev_posted(ctx);
		if (poll->events & EPOLLONESHOT)
			io_put_req(req);
	}
	return ipt.error;