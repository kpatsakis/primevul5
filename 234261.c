void cgit_clone_info(struct cgit_context *ctx)
{
	if (!ctx->qry.path || strcmp(ctx->qry.path, "refs"))
		return;

	ctx->page.mimetype = "text/plain";
	ctx->page.filename = "info/refs";
	cgit_print_http_headers(ctx);
	for_each_ref(print_ref_info, ctx);
}