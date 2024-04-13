static void tag_fn(struct cgit_context *ctx)
{
	cgit_print_tag(ctx->qry.sha1);
}