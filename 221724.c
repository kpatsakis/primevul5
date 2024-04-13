void fuse_unlock_inode(struct inode *inode, bool locked)
{
	if (locked)
		mutex_unlock(&get_fuse_inode(inode)->mutex);
}