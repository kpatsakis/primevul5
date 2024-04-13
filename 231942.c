static int ext4_add_entry(handle_t *handle, struct dentry *dentry,
			  struct inode *inode)
{
	struct inode *dir = d_inode(dentry->d_parent);
	struct buffer_head *bh = NULL;
	struct ext4_dir_entry_2 *de;
	struct super_block *sb;
#ifdef CONFIG_UNICODE
	struct ext4_sb_info *sbi;
#endif
	struct ext4_filename fname;
	int	retval;
	int	dx_fallback=0;
	unsigned blocksize;
	ext4_lblk_t block, blocks;
	int	csum_size = 0;

	if (ext4_has_metadata_csum(inode->i_sb))
		csum_size = sizeof(struct ext4_dir_entry_tail);

	sb = dir->i_sb;
	blocksize = sb->s_blocksize;
	if (!dentry->d_name.len)
		return -EINVAL;

#ifdef CONFIG_UNICODE
	sbi = EXT4_SB(sb);
	if (ext4_has_strict_mode(sbi) && IS_CASEFOLDED(dir) &&
	    sbi->s_encoding && utf8_validate(sbi->s_encoding, &dentry->d_name))
		return -EINVAL;
#endif

	retval = ext4_fname_setup_filename(dir, &dentry->d_name, 0, &fname);
	if (retval)
		return retval;

	if (ext4_has_inline_data(dir)) {
		retval = ext4_try_add_inline_entry(handle, &fname, dir, inode);
		if (retval < 0)
			goto out;
		if (retval == 1) {
			retval = 0;
			goto out;
		}
	}

	if (is_dx(dir)) {
		retval = ext4_dx_add_entry(handle, &fname, dir, inode);
		if (!retval || (retval != ERR_BAD_DX_DIR))
			goto out;
		/* Can we just ignore htree data? */
		if (ext4_has_metadata_csum(sb)) {
			EXT4_ERROR_INODE(dir,
				"Directory has corrupted htree index.");
			retval = -EFSCORRUPTED;
			goto out;
		}
		ext4_clear_inode_flag(dir, EXT4_INODE_INDEX);
		dx_fallback++;
		retval = ext4_mark_inode_dirty(handle, dir);
		if (unlikely(retval))
			goto out;
	}
	blocks = dir->i_size >> sb->s_blocksize_bits;
	for (block = 0; block < blocks; block++) {
		bh = ext4_read_dirblock(dir, block, DIRENT);
		if (bh == NULL) {
			bh = ext4_bread(handle, dir, block,
					EXT4_GET_BLOCKS_CREATE);
			goto add_to_new_block;
		}
		if (IS_ERR(bh)) {
			retval = PTR_ERR(bh);
			bh = NULL;
			goto out;
		}
		retval = add_dirent_to_buf(handle, &fname, dir, inode,
					   NULL, bh);
		if (retval != -ENOSPC)
			goto out;

		if (blocks == 1 && !dx_fallback &&
		    ext4_has_feature_dir_index(sb)) {
			retval = make_indexed_dir(handle, &fname, dir,
						  inode, bh);
			bh = NULL; /* make_indexed_dir releases bh */
			goto out;
		}
		brelse(bh);
	}
	bh = ext4_append(handle, dir, &block);
add_to_new_block:
	if (IS_ERR(bh)) {
		retval = PTR_ERR(bh);
		bh = NULL;
		goto out;
	}
	de = (struct ext4_dir_entry_2 *) bh->b_data;
	de->inode = 0;
	de->rec_len = ext4_rec_len_to_disk(blocksize - csum_size, blocksize);

	if (csum_size)
		ext4_initialize_dirent_tail(bh, blocksize);

	retval = add_dirent_to_buf(handle, &fname, dir, inode, de, bh);
out:
	ext4_fname_free_filename(&fname);
	brelse(bh);
	if (retval == 0)
		ext4_set_inode_state(inode, EXT4_STATE_NEWENTRY);
	return retval;
}