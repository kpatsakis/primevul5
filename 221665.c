static void fuse_dentry_release(struct dentry *dentry)
{
	union fuse_dentry *fd = dentry->d_fsdata;

	kfree_rcu(fd, rcu);
}