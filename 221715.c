static void fuse_sb_destroy(struct super_block *sb)
{
	struct fuse_mount *fm = get_fuse_mount_super(sb);
	bool last;

	if (fm) {
		last = fuse_mount_remove(fm);
		if (last)
			fuse_conn_destroy(fm);
	}
}