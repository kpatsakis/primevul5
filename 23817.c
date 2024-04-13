void cgit_add_clone_urls(void (*fn)(const char *))
{
	if (ctx.repo->clone_url)
		add_clone_urls(fn, expand_macros(ctx.repo->clone_url), NULL);
	else if (ctx.cfg.clone_prefix)
		add_clone_urls(fn, ctx.cfg.clone_prefix, ctx.repo->url);
}