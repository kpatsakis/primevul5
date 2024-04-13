void fuse_free_conn(struct fuse_conn *fc)
{
	WARN_ON(!list_empty(&fc->devices));
	kfree_rcu(fc, rcu);
}