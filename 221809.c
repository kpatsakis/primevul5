static inline struct fuse_mount *get_fuse_mount_super(struct super_block *sb)
{
	return sb->s_fs_info;
}