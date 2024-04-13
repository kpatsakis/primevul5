void lzw_context_destroy(struct lzw_ctx *ctx)
{
	free(ctx);
}