static int ext4_add_nondir(handle_t *handle,
		struct dentry *dentry, struct inode **inodep)
{
	struct inode *dir = d_inode(dentry->d_parent);
	struct inode *inode = *inodep;
	int err = ext4_add_entry(handle, dentry, inode);
	if (!err) {
		err = ext4_mark_inode_dirty(handle, inode);
		if (IS_DIRSYNC(dir))
			ext4_handle_sync(handle);
		d_instantiate_new(dentry, inode);
		*inodep = NULL;
		return err;
	}
	drop_nlink(inode);
	ext4_orphan_add(handle, inode);
	unlock_new_inode(inode);
	return err;
}