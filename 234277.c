static void send_file(struct cgit_context *ctx, char *path)
{
	struct stat st;
	int err;

	if (stat(path, &st)) {
		switch (errno) {
		case ENOENT:
			err = 404;
			break;
		case EACCES:
			err = 403;
			break;
		default:
			err = 400;
		}
		html_status(err, 0);
		return;
	}
	ctx->page.mimetype = "application/octet-stream";
	ctx->page.filename = path;
	if (prefixcmp(ctx->repo->path, path))
		ctx->page.filename += strlen(ctx->repo->path) + 1;
	cgit_print_http_headers(ctx);
	html_include(path);
}