bool ext4_empty_dir(struct inode *inode)
{
	unsigned int offset;
	struct buffer_head *bh;
	struct ext4_dir_entry_2 *de;
	struct super_block *sb;

	if (ext4_has_inline_data(inode)) {
		int has_inline_data = 1;
		int ret;

		ret = empty_inline_dir(inode, &has_inline_data);
		if (has_inline_data)
			return ret;
	}

	sb = inode->i_sb;
	if (inode->i_size < EXT4_DIR_REC_LEN(1) + EXT4_DIR_REC_LEN(2)) {
		EXT4_ERROR_INODE(inode, "invalid size");
		return true;
	}
	/* The first directory block must not be a hole,
	 * so treat it as DIRENT_HTREE
	 */
	bh = ext4_read_dirblock(inode, 0, DIRENT_HTREE);
	if (IS_ERR(bh))
		return true;

	de = (struct ext4_dir_entry_2 *) bh->b_data;
	if (ext4_check_dir_entry(inode, NULL, de, bh, bh->b_data, bh->b_size,
				 0) ||
	    le32_to_cpu(de->inode) != inode->i_ino || strcmp(".", de->name)) {
		ext4_warning_inode(inode, "directory missing '.'");
		brelse(bh);
		return true;
	}
	offset = ext4_rec_len_from_disk(de->rec_len, sb->s_blocksize);
	de = ext4_next_entry(de, sb->s_blocksize);
	if (ext4_check_dir_entry(inode, NULL, de, bh, bh->b_data, bh->b_size,
				 offset) ||
	    le32_to_cpu(de->inode) == 0 || strcmp("..", de->name)) {
		ext4_warning_inode(inode, "directory missing '..'");
		brelse(bh);
		return true;
	}
	offset += ext4_rec_len_from_disk(de->rec_len, sb->s_blocksize);
	while (offset < inode->i_size) {
		if (!(offset & (sb->s_blocksize - 1))) {
			unsigned int lblock;
			brelse(bh);
			lblock = offset >> EXT4_BLOCK_SIZE_BITS(sb);
			bh = ext4_read_dirblock(inode, lblock, EITHER);
			if (bh == NULL) {
				offset += sb->s_blocksize;
				continue;
			}
			if (IS_ERR(bh))
				return true;
		}
		de = (struct ext4_dir_entry_2 *) (bh->b_data +
					(offset & (sb->s_blocksize - 1)));
		if (ext4_check_dir_entry(inode, NULL, de, bh,
					 bh->b_data, bh->b_size, offset)) {
			offset = (offset | (sb->s_blocksize - 1)) + 1;
			continue;
		}
		if (le32_to_cpu(de->inode)) {
			brelse(bh);
			return false;
		}
		offset += ext4_rec_len_from_disk(de->rec_len, sb->s_blocksize);
	}
	brelse(bh);
	return true;
}