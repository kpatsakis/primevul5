 */
static int io_timeout_remove(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_timeout_rem *tr = &req->timeout_rem;
	struct io_ring_ctx *ctx = req->ctx;
	int ret;

	spin_lock_irq(&ctx->completion_lock);
	if (!(req->timeout_rem.flags & IORING_TIMEOUT_UPDATE))
		ret = io_timeout_cancel(ctx, tr->addr);
	else
		ret = io_timeout_update(ctx, tr->addr, &tr->ts,
					io_translate_timeout_mode(tr->flags));

	io_cqring_fill_event(ctx, req->user_data, ret, 0);
	io_commit_cqring(ctx);
	spin_unlock_irq(&ctx->completion_lock);
	io_cqring_ev_posted(ctx);
	if (ret < 0)
		req_set_fail_links(req);
	io_put_req(req);
	return 0;