
static int io_register_enable_rings(struct io_ring_ctx *ctx)
{
	if (!(ctx->flags & IORING_SETUP_R_DISABLED))
		return -EBADFD;

	if (ctx->restrictions.registered)
		ctx->restricted = 1;

	ctx->flags &= ~IORING_SETUP_R_DISABLED;
	if (ctx->sq_data && wq_has_sleeper(&ctx->sq_data->wait))
		wake_up(&ctx->sq_data->wait);
	return 0;