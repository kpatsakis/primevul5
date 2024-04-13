struct fuse_conn *fuse_conn_get(struct fuse_conn *fc)
{
	refcount_inc(&fc->count);
	return fc;
}