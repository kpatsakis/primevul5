void fuse_dev_install(struct fuse_dev *fud, struct fuse_conn *fc)
{
	fud->fc = fuse_conn_get(fc);
	spin_lock(&fc->lock);
	list_add_tail(&fud->entry, &fc->devices);
	spin_unlock(&fc->lock);
}