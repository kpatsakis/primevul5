
static void io_destroy_buffers(struct io_ring_ctx *ctx)
{
	struct io_buffer *buf;
	unsigned long index;

	xa_for_each(&ctx->io_buffers, index, buf)
		__io_remove_buffers(ctx, buf, index, -1U);