static void io_kill_timeouts(struct io_ring_ctx *ctx)
{
	struct io_kiocb *req, *tmp;

	spin_lock_irq(&ctx->completion_lock);
	list_for_each_entry_safe(req, tmp, &ctx->timeout_list, timeout.list)
		io_kill_timeout(req);
	spin_unlock_irq(&ctx->completion_lock);
}