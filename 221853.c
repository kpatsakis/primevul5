static inline struct fuse_mount *get_fuse_mount(struct inode *inode)
{
	return get_fuse_mount_super(inode->i_sb);
}