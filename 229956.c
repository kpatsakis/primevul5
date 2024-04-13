static fmode_t _nfs4_ctx_to_accessmode(const struct nfs_open_context *ctx)
{
	 return ctx->mode & (FMODE_READ|FMODE_WRITE|FMODE_EXEC);
}