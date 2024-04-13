static void blob_fn(struct cgit_context *ctx)
{
	cgit_print_blob(ctx->qry.sha1, ctx->qry.path, ctx->qry.head);
}