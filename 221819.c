static void fuse_umount_begin(struct super_block *sb)
{
	struct fuse_conn *fc = get_fuse_conn_super(sb);

	if (!fc->no_force_umount)
		fuse_abort_conn(fc);
}