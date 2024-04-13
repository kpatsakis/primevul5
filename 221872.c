static inline void __fuse_dentry_settime(struct dentry *dentry, u64 time)
{
	((union fuse_dentry *) dentry->d_fsdata)->time = time;
}