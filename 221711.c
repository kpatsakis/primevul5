static bool fuse_range_is_writeback(struct inode *inode, pgoff_t idx_from,
				   pgoff_t idx_to)
{
	struct fuse_inode *fi = get_fuse_inode(inode);
	bool found;

	spin_lock(&fi->lock);
	found = fuse_find_writeback(fi, idx_from, idx_to);
	spin_unlock(&fi->lock);

	return found;
}