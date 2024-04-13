static int fuse_reconfigure(struct fs_context *fc)
{
	struct super_block *sb = fc->root->d_sb;

	sync_filesystem(sb);
	if (fc->sb_flags & SB_MANDLOCK)
		return -EINVAL;

	return 0;
}