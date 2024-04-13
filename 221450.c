
static int io_req_defer(struct io_kiocb *req)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct io_defer_entry *de;
	int ret;
	u32 seq;

	/* Still need defer if there is pending req in defer list. */
	if (likely(list_empty_careful(&ctx->defer_list) &&
		!(req->flags & REQ_F_IO_DRAIN)))
		return 0;

	seq = io_get_sequence(req);
	/* Still a chance to pass the sequence check */
	if (!req_need_defer(req, seq) && list_empty_careful(&ctx->defer_list))
		return 0;

	ret = io_req_prep_async(req);
	if (ret)
		return ret;
	io_prep_async_link(req);
	de = kmalloc(sizeof(*de), GFP_KERNEL);
	if (!de)
		return -ENOMEM;

	spin_lock_irq(&ctx->completion_lock);
	if (!req_need_defer(req, seq) && list_empty(&ctx->defer_list)) {
		spin_unlock_irq(&ctx->completion_lock);
		kfree(de);
		io_queue_async_work(req);
		return -EIOCBQUEUED;
	}

	trace_io_uring_defer(ctx, req, req->user_data);
	de->req = req;
	de->seq = seq;
	list_add_tail(&de->list, &ctx->defer_list);
	spin_unlock_irq(&ctx->completion_lock);
	return -EIOCBQUEUED;