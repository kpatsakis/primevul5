static void diff_fn(struct cgit_context *ctx)
{
	cgit_print_diff(ctx->qry.sha1, ctx->qry.sha2, ctx->qry.path);
}