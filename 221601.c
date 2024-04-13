static inline bool io_should_trigger_evfd(struct io_ring_ctx *ctx)
{
	if (likely(!ctx->cq_ev_fd))
		return false;
	if (READ_ONCE(ctx->rings->cq_flags) & IORING_CQ_EVENTFD_DISABLED)
		return false;
	return !ctx->eventfd_async || io_wq_current_is_worker();
}