static int fuse_init_fs_context(struct fs_context *fc)
{
	struct fuse_fs_context *ctx;

	ctx = kzalloc(sizeof(struct fuse_fs_context), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->max_read = ~0;
	ctx->blksize = FUSE_DEFAULT_BLKSIZE;
	ctx->legacy_opts_show = true;

#ifdef CONFIG_BLOCK
	if (fc->fs_type == &fuseblk_fs_type) {
		ctx->is_bdev = true;
		ctx->destroy = true;
	}
#endif

	fc->fs_private = ctx;
	fc->ops = &fuse_context_ops;
	return 0;
}