static void commit_fn(struct cgit_context *ctx)
{
	cgit_print_commit(ctx->qry.sha1);
}