
static void io_rsrc_buf_put(struct io_ring_ctx *ctx, struct io_rsrc_put *prsrc)
{
	io_buffer_unmap(ctx, &prsrc->buf);
	prsrc->buf = NULL;