void fuse_update_ctime(struct inode *inode)
{
	if (!IS_NOCMTIME(inode)) {
		inode->i_ctime = current_time(inode);
		mark_inode_dirty_sync(inode);
	}
}