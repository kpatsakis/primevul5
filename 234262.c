static void snapshot_fn(struct cgit_context *ctx)
{
	cgit_print_snapshot(ctx->qry.head, ctx->qry.sha1,
			    cgit_repobasename(ctx->repo->url), ctx->qry.path,
			    ctx->repo->snapshots);
}