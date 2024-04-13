static void patch_fn(struct cgit_context *ctx)
{
	cgit_print_patch(ctx->qry.sha1);
}