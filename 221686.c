void fuse_invalidate_attr(struct inode *inode)
{
	fuse_invalidate_attr_mask(inode, STATX_BASIC_STATS);
}