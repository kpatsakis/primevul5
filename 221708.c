static int fuse_writeback_range(struct inode *inode, loff_t start, loff_t end)
{
	int err = filemap_write_and_wait_range(inode->i_mapping, start, end);

	if (!err)
		fuse_sync_writes(inode);

	return err;
}