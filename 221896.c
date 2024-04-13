static int fuse_dentry_init(struct dentry *dentry)
{
	dentry->d_fsdata = kzalloc(sizeof(union fuse_dentry),
				   GFP_KERNEL_ACCOUNT | __GFP_RECLAIMABLE);

	return dentry->d_fsdata ? 0 : -ENOMEM;
}