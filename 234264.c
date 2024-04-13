void cgit_clone_objects(struct cgit_context *ctx)
{
	if (!ctx->qry.path) {
		html_status(400, 0);
		return;
	}

	if (!strcmp(ctx->qry.path, "info/packs")) {
		print_pack_info(ctx);
		return;
	}

	send_file(ctx, git_path("objects/%s", ctx->qry.path));
}