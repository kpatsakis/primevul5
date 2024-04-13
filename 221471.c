
static int io_sqe_files_unregister(struct io_ring_ctx *ctx)
{
	int ret;

	if (!ctx->file_data)
		return -ENXIO;
	ret = io_rsrc_ref_quiesce(ctx->file_data, ctx);
	if (!ret)
		__io_sqe_files_unregister(ctx);
	return ret;