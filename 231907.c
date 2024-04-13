static void ext4_inc_count(handle_t *handle, struct inode *inode)
{
	inc_nlink(inode);
	if (is_dx(inode) &&
	    (inode->i_nlink > EXT4_LINK_MAX || inode->i_nlink == 2))
		set_nlink(inode, 1);
}