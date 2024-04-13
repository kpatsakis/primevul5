static void about_fn(struct cgit_context *ctx)
{
	if (ctx->repo)
		cgit_print_repo_readme();
	else
		cgit_print_site_readme();
}