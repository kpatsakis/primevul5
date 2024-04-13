
static inline bool io_run_ctx_fallback(struct io_ring_ctx *ctx)
{
	return io_run_task_work_head(&ctx->exit_task_work);