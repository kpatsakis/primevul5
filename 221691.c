static int fuse_get_tree(struct fs_context *fc)
{
	struct fuse_fs_context *ctx = fc->fs_private;

	if (!ctx->fd_present || !ctx->rootmode_present ||
	    !ctx->user_id_present || !ctx->group_id_present)
		return -EINVAL;

#ifdef CONFIG_BLOCK
	if (ctx->is_bdev)
		return get_tree_bdev(fc, fuse_fill_super);
#endif

	return get_tree_nodev(fc, fuse_fill_super);
}