void fuse_release_nowrite(struct inode *inode)
{
	struct fuse_inode *fi = get_fuse_inode(inode);

	spin_lock(&fi->lock);
	__fuse_release_nowrite(inode);
	spin_unlock(&fi->lock);
}