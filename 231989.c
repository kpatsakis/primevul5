int ext4_handle_dirty_dirblock(handle_t *handle,
			       struct inode *inode,
			       struct buffer_head *bh)
{
	ext4_dirblock_csum_set(inode, bh);
	return ext4_handle_dirty_metadata(handle, inode, bh);
}