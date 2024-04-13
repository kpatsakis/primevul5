static int ext4_symlink(struct inode *dir,
			struct dentry *dentry, const char *symname)
{
	handle_t *handle;
	struct inode *inode;
	int err, len = strlen(symname);
	int credits;
	struct fscrypt_str disk_link;

	if (unlikely(ext4_forced_shutdown(EXT4_SB(dir->i_sb))))
		return -EIO;

	err = fscrypt_prepare_symlink(dir, symname, len, dir->i_sb->s_blocksize,
				      &disk_link);
	if (err)
		return err;

	err = dquot_initialize(dir);
	if (err)
		return err;

	if ((disk_link.len > EXT4_N_BLOCKS * 4)) {
		/*
		 * For non-fast symlinks, we just allocate inode and put it on
		 * orphan list in the first transaction => we need bitmap,
		 * group descriptor, sb, inode block, quota blocks, and
		 * possibly selinux xattr blocks.
		 */
		credits = 4 + EXT4_MAXQUOTAS_INIT_BLOCKS(dir->i_sb) +
			  EXT4_XATTR_TRANS_BLOCKS;
	} else {
		/*
		 * Fast symlink. We have to add entry to directory
		 * (EXT4_DATA_TRANS_BLOCKS + EXT4_INDEX_EXTRA_TRANS_BLOCKS),
		 * allocate new inode (bitmap, group descriptor, inode block,
		 * quota blocks, sb is already counted in previous macros).
		 */
		credits = EXT4_DATA_TRANS_BLOCKS(dir->i_sb) +
			  EXT4_INDEX_EXTRA_TRANS_BLOCKS + 3;
	}

	inode = ext4_new_inode_start_handle(dir, S_IFLNK|S_IRWXUGO,
					    &dentry->d_name, 0, NULL,
					    EXT4_HT_DIR, credits);
	handle = ext4_journal_current_handle();
	if (IS_ERR(inode)) {
		if (handle)
			ext4_journal_stop(handle);
		return PTR_ERR(inode);
	}

	if (IS_ENCRYPTED(inode)) {
		err = fscrypt_encrypt_symlink(inode, symname, len, &disk_link);
		if (err)
			goto err_drop_inode;
		inode->i_op = &ext4_encrypted_symlink_inode_operations;
	}

	if ((disk_link.len > EXT4_N_BLOCKS * 4)) {
		if (!IS_ENCRYPTED(inode))
			inode->i_op = &ext4_symlink_inode_operations;
		inode_nohighmem(inode);
		ext4_set_aops(inode);
		/*
		 * We cannot call page_symlink() with transaction started
		 * because it calls into ext4_write_begin() which can wait
		 * for transaction commit if we are running out of space
		 * and thus we deadlock. So we have to stop transaction now
		 * and restart it when symlink contents is written.
		 * 
		 * To keep fs consistent in case of crash, we have to put inode
		 * to orphan list in the mean time.
		 */
		drop_nlink(inode);
		err = ext4_orphan_add(handle, inode);
		ext4_journal_stop(handle);
		handle = NULL;
		if (err)
			goto err_drop_inode;
		err = __page_symlink(inode, disk_link.name, disk_link.len, 1);
		if (err)
			goto err_drop_inode;
		/*
		 * Now inode is being linked into dir (EXT4_DATA_TRANS_BLOCKS
		 * + EXT4_INDEX_EXTRA_TRANS_BLOCKS), inode is also modified
		 */
		handle = ext4_journal_start(dir, EXT4_HT_DIR,
				EXT4_DATA_TRANS_BLOCKS(dir->i_sb) +
				EXT4_INDEX_EXTRA_TRANS_BLOCKS + 1);
		if (IS_ERR(handle)) {
			err = PTR_ERR(handle);
			handle = NULL;
			goto err_drop_inode;
		}
		set_nlink(inode, 1);
		err = ext4_orphan_del(handle, inode);
		if (err)
			goto err_drop_inode;
	} else {
		/* clear the extent format for fast symlink */
		ext4_clear_inode_flag(inode, EXT4_INODE_EXTENTS);
		if (!IS_ENCRYPTED(inode)) {
			inode->i_op = &ext4_fast_symlink_inode_operations;
			inode->i_link = (char *)&EXT4_I(inode)->i_data;
		}
		memcpy((char *)&EXT4_I(inode)->i_data, disk_link.name,
		       disk_link.len);
		inode->i_size = disk_link.len - 1;
	}
	EXT4_I(inode)->i_disksize = inode->i_size;
	err = ext4_add_nondir(handle, dentry, &inode);
	if (handle)
		ext4_journal_stop(handle);
	if (inode)
		iput(inode);
	goto out_free_encrypted_link;

err_drop_inode:
	if (handle)
		ext4_journal_stop(handle);
	clear_nlink(inode);
	unlock_new_inode(inode);
	iput(inode);
out_free_encrypted_link:
	if (disk_link.name != (unsigned char *)symname)
		kfree(disk_link.name);
	return err;
}