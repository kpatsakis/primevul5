static int ext4_rename(struct inode *old_dir, struct dentry *old_dentry,
		       struct inode *new_dir, struct dentry *new_dentry,
		       unsigned int flags)
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
	int force_reread;
	int retval;
	struct inode *whiteout = NULL;
	int credits;
	u8 old_file_type;

	if (new.inode && new.inode->i_nlink == 0) {
		EXT4_ERROR_INODE(new.inode,
				 "target of rename is already freed");
		return -EFSCORRUPTED;
	}

	if ((ext4_test_inode_flag(new_dir, EXT4_INODE_PROJINHERIT)) &&
	    (!projid_eq(EXT4_I(new_dir)->i_projid,
			EXT4_I(old_dentry->d_inode)->i_projid)))
		return -EXDEV;

	retval = dquot_initialize(old.dir);
	if (retval)
		return retval;
	retval = dquot_initialize(new.dir);
	if (retval)
		return retval;

	/* Initialize quotas before so that eventual writes go
	 * in separate transaction */
	if (new.inode) {
		retval = dquot_initialize(new.inode);
		if (retval)
			return retval;
	}

	old.bh = ext4_find_entry(old.dir, &old.dentry->d_name, &old.de, NULL);
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
	if (new.bh) {
		if (!new.inode) {
			brelse(new.bh);
			new.bh = NULL;
		}
	}
	if (new.inode && !test_opt(new.dir->i_sb, NO_AUTO_DA_ALLOC))
		ext4_alloc_da_blocks(old.inode);

	credits = (2 * EXT4_DATA_TRANS_BLOCKS(old.dir->i_sb) +
		   EXT4_INDEX_EXTRA_TRANS_BLOCKS + 2);
	if (!(flags & RENAME_WHITEOUT)) {
		handle = ext4_journal_start(old.dir, EXT4_HT_DIR, credits);
		if (IS_ERR(handle)) {
			retval = PTR_ERR(handle);
			handle = NULL;
			goto end_rename;
		}
	} else {
		whiteout = ext4_whiteout_for_rename(&old, credits, &handle);
		if (IS_ERR(whiteout)) {
			retval = PTR_ERR(whiteout);
			whiteout = NULL;
			goto end_rename;
		}
	}

	if (IS_DIRSYNC(old.dir) || IS_DIRSYNC(new.dir))
		ext4_handle_sync(handle);

	if (S_ISDIR(old.inode->i_mode)) {
		if (new.inode) {
			retval = -ENOTEMPTY;
			if (!ext4_empty_dir(new.inode))
				goto end_rename;
		} else {
			retval = -EMLINK;
			if (new.dir != old.dir && EXT4_DIR_LINK_MAX(new.dir))
				goto end_rename;
		}
		retval = ext4_rename_dir_prepare(handle, &old);
		if (retval)
			goto end_rename;
	}
	/*
	 * If we're renaming a file within an inline_data dir and adding or
	 * setting the new dirent causes a conversion from inline_data to
	 * extents/blockmap, we need to force the dirent delete code to
	 * re-read the directory, or else we end up trying to delete a dirent
	 * from what is now the extent tree root (or a block map).
	 */
	force_reread = (new.dir->i_ino == old.dir->i_ino &&
			ext4_test_inode_flag(new.dir, EXT4_INODE_INLINE_DATA));

	old_file_type = old.de->file_type;
	if (whiteout) {
		/*
		 * Do this before adding a new entry, so the old entry is sure
		 * to be still pointing to the valid old entry.
		 */
		retval = ext4_setent(handle, &old, whiteout->i_ino,
				     EXT4_FT_CHRDEV);
		if (retval)
			goto end_rename;
		retval = ext4_mark_inode_dirty(handle, whiteout);
		if (unlikely(retval))
			goto end_rename;
	}
	if (!new.bh) {
		retval = ext4_add_entry(handle, new.dentry, old.inode);
		if (retval)
			goto end_rename;
	} else {
		retval = ext4_setent(handle, &new,
				     old.inode->i_ino, old_file_type);
		if (retval)
			goto end_rename;
	}
	if (force_reread)
		force_reread = !ext4_test_inode_flag(new.dir,
						     EXT4_INODE_INLINE_DATA);

	/*
	 * Like most other Unix systems, set the ctime for inodes on a
	 * rename.
	 */
	old.inode->i_ctime = current_time(old.inode);
	retval = ext4_mark_inode_dirty(handle, old.inode);
	if (unlikely(retval))
		goto end_rename;

	if (!whiteout) {
		/*
		 * ok, that's it
		 */
		ext4_rename_delete(handle, &old, force_reread);
	}

	if (new.inode) {
		ext4_dec_count(handle, new.inode);
		new.inode->i_ctime = current_time(new.inode);
	}
	old.dir->i_ctime = old.dir->i_mtime = current_time(old.dir);
	ext4_update_dx_flag(old.dir);
	if (old.dir_bh) {
		retval = ext4_rename_dir_finish(handle, &old, new.dir->i_ino);
		if (retval)
			goto end_rename;

		ext4_dec_count(handle, old.dir);
		if (new.inode) {
			/* checked ext4_empty_dir above, can't have another
			 * parent, ext4_dec_count() won't work for many-linked
			 * dirs */
			clear_nlink(new.inode);
		} else {
			ext4_inc_count(handle, new.dir);
			ext4_update_dx_flag(new.dir);
			retval = ext4_mark_inode_dirty(handle, new.dir);
			if (unlikely(retval))
				goto end_rename;
		}
	}
	retval = ext4_mark_inode_dirty(handle, old.dir);
	if (unlikely(retval))
		goto end_rename;
	if (new.inode) {
		retval = ext4_mark_inode_dirty(handle, new.inode);
		if (unlikely(retval))
			goto end_rename;
		if (!new.inode->i_nlink)
			ext4_orphan_add(handle, new.inode);
	}
	retval = 0;

end_rename:
	brelse(old.dir_bh);
	brelse(old.bh);
	brelse(new.bh);
	if (whiteout) {
		if (retval)
			drop_nlink(whiteout);
		unlock_new_inode(whiteout);
		iput(whiteout);
	}
	if (handle)
		ext4_journal_stop(handle);
	return retval;
}