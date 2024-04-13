static int fuse_dentry_delete(const struct dentry *dentry)
{
	return time_before64(fuse_dentry_time(dentry), get_jiffies_64());
}