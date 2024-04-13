static inline struct fuse_conn *get_fuse_conn_super(struct super_block *sb)
{
	return get_fuse_mount_super(sb)->fc;
}