
static bool io_arm_poll_handler(struct io_kiocb *req)
{
	const struct io_op_def *def = &io_op_defs[req->opcode];
	struct io_ring_ctx *ctx = req->ctx;
	struct async_poll *apoll;
	struct io_poll_table ipt;
	__poll_t mask, ret;
	int rw;

	if (!req->file || !file_can_poll(req->file))
		return false;
	if (req->flags & REQ_F_POLLED)
		return false;
	if (def->pollin)
		rw = READ;
	else if (def->pollout)
		rw = WRITE;
	else
		return false;
	/* if we can't nonblock try, then no point in arming a poll handler */
	if (!io_file_supports_async(req, rw))
		return false;

	apoll = kmalloc(sizeof(*apoll), GFP_ATOMIC);
	if (unlikely(!apoll))
		return false;
	apoll->double_poll = NULL;

	req->flags |= REQ_F_POLLED;
	req->apoll = apoll;

	mask = EPOLLONESHOT;
	if (def->pollin)
		mask |= POLLIN | POLLRDNORM;
	if (def->pollout)
		mask |= POLLOUT | POLLWRNORM;

	/* If reading from MSG_ERRQUEUE using recvmsg, ignore POLLIN */
	if ((req->opcode == IORING_OP_RECVMSG) &&
	    (req->sr_msg.msg_flags & MSG_ERRQUEUE))
		mask &= ~POLLIN;

	mask |= POLLERR | POLLPRI;

	ipt.pt._qproc = io_async_queue_proc;

	ret = __io_arm_poll_handler(req, &apoll->poll, &ipt, mask,
					io_async_wake);
	if (ret || ipt.error) {
		io_poll_remove_double(req);
		spin_unlock_irq(&ctx->completion_lock);
		return false;
	}
	spin_unlock_irq(&ctx->completion_lock);
	trace_io_uring_poll_arm(ctx, req->opcode, req->user_data, mask,
					apoll->poll.events);
	return true;