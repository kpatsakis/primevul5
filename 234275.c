void cgit_clone_head(struct cgit_context *ctx)
{
	send_file(ctx, git_path("%s", "HEAD"));
}