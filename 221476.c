
static void __io_sqe_buffers_unregister(struct io_ring_ctx *ctx)
{
	unsigned int i;

	for (i = 0; i < ctx->nr_user_bufs; i++)
		io_buffer_unmap(ctx, &ctx->user_bufs[i]);
	kfree(ctx->user_bufs);
	io_rsrc_data_free(ctx->buf_data);
	ctx->user_bufs = NULL;
	ctx->buf_data = NULL;
	ctx->nr_user_bufs = 0;