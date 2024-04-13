static inline u64 fuse_dentry_time(const struct dentry *entry)
{
	return (u64)entry->d_fsdata;
}