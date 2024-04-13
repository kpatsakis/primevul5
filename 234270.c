static void ls_cache_fn(struct cgit_context *ctx)
{
	ctx->page.mimetype = "text/plain";
	ctx->page.filename = "ls-cache.txt";
	cgit_print_http_headers(ctx);
	cache_ls(ctx->cfg.cache_root);
}