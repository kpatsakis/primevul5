
static int io_buffers_map_alloc(struct io_ring_ctx *ctx, unsigned int nr_args)
{
	ctx->user_bufs = kcalloc(nr_args, sizeof(*ctx->user_bufs), GFP_KERNEL);
	return ctx->user_bufs ? 0 : -ENOMEM;