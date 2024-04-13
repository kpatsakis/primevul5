struct fuse_file *fuse_file_get(struct fuse_file *ff)
{
	atomic_inc(&ff->count);
	return ff;
}