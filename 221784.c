void fuse_sync_release(struct fuse_inode *fi, struct fuse_file *ff, int flags)
{
	WARN_ON(refcount_read(&ff->count) > 1);
	fuse_prepare_release(fi, ff, flags, FUSE_RELEASE);
	/*
	 * iput(NULL) is a no-op and since the refcount is 1 and everything's
	 * synchronous, we are fine with not doing igrab() here"
	 */
	fuse_file_put(ff, true, false);
}