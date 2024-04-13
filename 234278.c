static void tree_fn(struct cgit_context *ctx)
{
	cgit_print_tree(ctx->qry.sha1, ctx->qry.path);
}