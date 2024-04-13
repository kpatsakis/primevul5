static void fuse_put_super(struct super_block *sb)
{
	struct fuse_mount *fm = get_fuse_mount_super(sb);

	fuse_conn_put(fm->fc);
	kfree(fm);
}