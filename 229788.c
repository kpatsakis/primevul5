static fmode_t _nfs4_ctx_to_openmode(const struct nfs_open_context *ctx)
{
	fmode_t ret = ctx->mode & (FMODE_READ|FMODE_WRITE);

	return (ctx->mode & FMODE_EXEC) ? FMODE_READ | ret : ret;
}