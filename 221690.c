int fuse_reverse_inval_inode(struct fuse_conn *fc, u64 nodeid,
			     loff_t offset, loff_t len)
{
	struct fuse_inode *fi;
	struct inode *inode;
	pgoff_t pg_start;
	pgoff_t pg_end;

	inode = fuse_ilookup(fc, nodeid, NULL);
	if (!inode)
		return -ENOENT;

	fi = get_fuse_inode(inode);
	spin_lock(&fi->lock);
	fi->attr_version = atomic64_inc_return(&fc->attr_version);
	spin_unlock(&fi->lock);

	fuse_invalidate_attr(inode);
	forget_all_cached_acls(inode);
	if (offset >= 0) {
		pg_start = offset >> PAGE_SHIFT;
		if (len <= 0)
			pg_end = -1;
		else
			pg_end = (offset + len - 1) >> PAGE_SHIFT;
		invalidate_inode_pages2_range(inode->i_mapping,
					      pg_start, pg_end);
	}
	iput(inode);
	return 0;
}