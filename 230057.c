static void nfs4_close_context(struct nfs_open_context *ctx, int is_sync)
{
	if (ctx->state == NULL)
		return;
	if (is_sync)
		nfs4_close_sync(ctx->state, _nfs4_ctx_to_openmode(ctx));
	else
		nfs4_close_state(ctx->state, _nfs4_ctx_to_openmode(ctx));
}