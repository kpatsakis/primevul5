static bool io_arm_poll_handler(struct io_kiocb *req)
{
	const struct io_op_def *def = &io_op_defs[req->opcode];
	struct io_ring_ctx *ctx = req->ctx;
	struct async_poll *apoll;
	struct io_poll_table ipt;
	__poll_t mask, ret;

	if (!req->file || !file_can_poll(req->file))
		return false;
	if (req->flags & REQ_F_POLLED)
		return false;
	if (!def->pollin && !def->pollout)
		return false;

	apoll = kmalloc(sizeof(*apoll), GFP_ATOMIC);
	if (unlikely(!apoll))
		return false;
	apoll->double_poll = NULL;

	req->flags |= REQ_F_POLLED;
	io_get_req_task(req);
	req->apoll = apoll;
	INIT_HLIST_NODE(&req->hash_node);

	mask = 0;
	if (def->pollin)
		mask |= POLLIN | POLLRDNORM;
	if (def->pollout)
		mask |= POLLOUT | POLLWRNORM;
	mask |= POLLERR | POLLPRI;

	ipt.pt._qproc = io_async_queue_proc;

	ret = __io_arm_poll_handler(req, &apoll->poll, &ipt, mask,
					io_async_wake);
	if (ret) {
		io_poll_remove_double(req, apoll->double_poll);
		spin_unlock_irq(&ctx->completion_lock);
		kfree(apoll->double_poll);
		kfree(apoll);
		return false;
	}
	spin_unlock_irq(&ctx->completion_lock);
	trace_io_uring_poll_arm(ctx, req->opcode, req->user_data, mask,
					apoll->poll.events);
	return true;
}