
static int io_rsrc_node_switch_start(struct io_ring_ctx *ctx)
{
	if (ctx->rsrc_backup_node)
		return 0;
	ctx->rsrc_backup_node = io_rsrc_node_alloc(ctx);
	return ctx->rsrc_backup_node ? 0 : -ENOMEM;