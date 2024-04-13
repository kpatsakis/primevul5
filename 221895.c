static void fuse_free_fc(struct fs_context *fc)
{
	struct fuse_fs_context *ctx = fc->fs_private;

	if (ctx) {
		kfree(ctx->subtype);
		kfree(ctx);
	}
}