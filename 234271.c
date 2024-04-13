static void log_fn(struct cgit_context *ctx)
{
	cgit_print_log(ctx->qry.sha1, ctx->qry.ofs, ctx->cfg.max_commit_count,
		       ctx->qry.grep, ctx->qry.search, ctx->qry.path, 1);
}