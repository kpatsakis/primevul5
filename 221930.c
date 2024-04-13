static inline u64 fuse_get_attr_version(struct fuse_conn *fc)
{
	return atomic64_read(&fc->attr_version);
}