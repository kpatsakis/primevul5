
static int io_sqe_buffers_unregister(struct io_ring_ctx *ctx)
{
	int ret;

	if (!ctx->buf_data)
		return -ENXIO;

	ret = io_rsrc_ref_quiesce(ctx->buf_data, ctx);
	if (!ret)
		__io_sqe_buffers_unregister(ctx);
	return ret;