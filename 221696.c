static inline struct fuse_conn *get_fuse_conn(struct inode *inode)
{
	return get_fuse_mount_super(inode->i_sb)->fc;
}