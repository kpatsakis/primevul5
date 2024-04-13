static inline u64 fuse_dentry_time(const struct dentry *entry)
{
	return ((union fuse_dentry *) entry->d_fsdata)->time;
}