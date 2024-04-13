static int ext4_cross_rename(struct inode *old_dir, struct dentry *old_dentry,
			     struct inode *new_dir, struct dentry *new_dentry)
{
	handle_t *handle = NULL;
	struct ext4_renament old = {
		.dir = old_dir,
		.dentry = old_dentry,
		.inode = d_inode(old_dentry),
	};
	struct ext4_renament new = {
		.dir = new_dir,
		.dentry = new_dentry,
		.inode = d_inode(new_dentry),
	};
	u8 new_file_type;
	int retval;
	struct timespec64 ctime;

	if ((ext4_test_inode_flag(new_dir, EXT4_INODE_PROJINHERIT) &&
	     !projid_eq(EXT4_I(new_dir)->i_projid,
			EXT4_I(old_dentry->d_inode)->i_projid)) ||
	    (ext4_test_inode_flag(old_dir, EXT4_INODE_PROJINHERIT) &&
	     !projid_eq(EXT4_I(old_dir)->i_projid,
			EXT4_I(new_dentry->d_inode)->i_projid)))
		return -EXDEV;

	retval = dquot_initialize(old.dir);
	if (retval)
		return retval;
	retval = dquot_initialize(new.dir);
	if (retval)
		return retval;

	old.bh = ext4_find_entry(old.dir, &old.dentry->d_name,
				 &old.de, &old.inlined);
	if (IS_ERR(old.bh))
		return PTR_ERR(old.bh);
	/*
	 *  Check for inode number is _not_ due to possible IO errors.
	 *  We might rmdir the source, keep it as pwd of some process
	 *  and merrily kill the link to whatever was created under the
	 *  same name. Goodbye sticky bit ;-<
	 */
	retval = -ENOENT;
	if (!old.bh || le32_to_cpu(old.de->inode) != old.inode->i_ino)
		goto end_rename;

	new.bh = ext4_find_entry(new.dir, &new.dentry->d_name,
				 &new.de, &new.inlined);
	if (IS_ERR(new.bh)) {
		retval = PTR_ERR(new.bh);
		new.bh = NULL;
		goto end_rename;
	}

	/* RENAME_EXCHANGE case: old *and* new must both exist */
	if (!new.bh || le32_to_cpu(new.de->inode) != new.inode->i_ino)
		goto end_rename;

	handle = ext4_journal_start(old.dir, EXT4_HT_DIR,
		(2 * EXT4_DATA_TRANS_BLOCKS(old.dir->i_sb) +
		 2 * EXT4_INDEX_EXTRA_TRANS_BLOCKS + 2));
	if (IS_ERR(handle)) {
		retval = PTR_ERR(handle);
		handle = NULL;
		goto end_rename;
	}

	if (IS_DIRSYNC(old.dir) || IS_DIRSYNC(new.dir))
		ext4_handle_sync(handle);

	if (S_ISDIR(old.inode->i_mode)) {
		old.is_dir = true;
		retval = ext4_rename_dir_prepare(handle, &old);
		if (retval)
			goto end_rename;
	}
	if (S_ISDIR(new.inode->i_mode)) {
		new.is_dir = true;
		retval = ext4_rename_dir_prepare(handle, &new);
		if (retval)
			goto end_rename;
	}

	/*
	 * Other than the special case of overwriting a directory, parents'
	 * nlink only needs to be modified if this is a cross directory rename.
	 */
	if (old.dir != new.dir && old.is_dir != new.is_dir) {
		old.dir_nlink_delta = old.is_dir ? -1 : 1;
		new.dir_nlink_delta = -old.dir_nlink_delta;
		retval = -EMLINK;
		if ((old.dir_nlink_delta > 0 && EXT4_DIR_LINK_MAX(old.dir)) ||
		    (new.dir_nlink_delta > 0 && EXT4_DIR_LINK_MAX(new.dir)))
			goto end_rename;
	}

	new_file_type = new.de->file_type;
	retval = ext4_setent(handle, &new, old.inode->i_ino, old.de->file_type);
	if (retval)
		goto end_rename;

	retval = ext4_setent(handle, &old, new.inode->i_ino, new_file_type);
	if (retval)
		goto end_rename;

	/*
	 * Like most other Unix systems, set the ctime for inodes on a
	 * rename.
	 */
	ctime = current_time(old.inode);
	old.inode->i_ctime = ctime;
	new.inode->i_ctime = ctime;
	retval = ext4_mark_inode_dirty(handle, old.inode);
	if (unlikely(retval))
		goto end_rename;
	retval = ext4_mark_inode_dirty(handle, new.inode);
	if (unlikely(retval))
		goto end_rename;

	if (old.dir_bh) {
		retval = ext4_rename_dir_finish(handle, &old, new.dir->i_ino);
		if (retval)
			goto end_rename;
	}
	if (new.dir_bh) {
		retval = ext4_rename_dir_finish(handle, &new, old.dir->i_ino);
		if (retval)
			goto end_rename;
	}
	ext4_update_dir_count(handle, &old);
	ext4_update_dir_count(handle, &new);
	retval = 0;

end_rename:
	brelse(old.dir_bh);
	brelse(new.dir_bh);
	brelse(old.bh);
	brelse(new.bh);
	if (handle)
		ext4_journal_stop(handle);
	return retval;
}