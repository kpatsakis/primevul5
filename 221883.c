static inline void __fuse_dentry_settime(struct dentry *entry, u64 time)
{
	entry->d_fsdata = (void *) time;
}