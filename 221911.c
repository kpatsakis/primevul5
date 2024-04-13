bool fuse_lock_inode(struct inode *inode)
{
	bool locked = false;

	if (!get_fuse_conn(inode)->parallel_dirops) {
		mutex_lock(&get_fuse_inode(inode)->mutex);
		locked = true;
	}

	return locked;
}