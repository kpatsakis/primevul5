struct fuse_dev *fuse_dev_alloc_install(struct fuse_conn *fc)
{
	struct fuse_dev *fud;

	fud = fuse_dev_alloc();
	if (!fud)
		return NULL;

	fuse_dev_install(fud, fc);
	return fud;
}