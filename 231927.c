static struct buffer_head *__ext4_read_dirblock(struct inode *inode,
						ext4_lblk_t block,
						dirblock_type_t type,
						const char *func,
						unsigned int line)
{
	struct buffer_head *bh;
	struct ext4_dir_entry *dirent;
	int is_dx_block = 0;

	if (ext4_simulate_fail(inode->i_sb, EXT4_SIM_DIRBLOCK_EIO))
		bh = ERR_PTR(-EIO);
	else
		bh = ext4_bread(NULL, inode, block, 0);
	if (IS_ERR(bh)) {
		__ext4_warning(inode->i_sb, func, line,
			       "inode #%lu: lblock %lu: comm %s: "
			       "error %ld reading directory block",
			       inode->i_ino, (unsigned long)block,
			       current->comm, PTR_ERR(bh));

		return bh;
	}
	if (!bh && (type == INDEX || type == DIRENT_HTREE)) {
		ext4_error_inode(inode, func, line, block,
				 "Directory hole found for htree %s block",
				 (type == INDEX) ? "index" : "leaf");
		return ERR_PTR(-EFSCORRUPTED);
	}
	if (!bh)
		return NULL;
	dirent = (struct ext4_dir_entry *) bh->b_data;
	/* Determine whether or not we have an index block */
	if (is_dx(inode)) {
		if (block == 0)
			is_dx_block = 1;
		else if (ext4_rec_len_from_disk(dirent->rec_len,
						inode->i_sb->s_blocksize) ==
			 inode->i_sb->s_blocksize)
			is_dx_block = 1;
	}
	if (!is_dx_block && type == INDEX) {
		ext4_error_inode(inode, func, line, block,
		       "directory leaf block found instead of index block");
		brelse(bh);
		return ERR_PTR(-EFSCORRUPTED);
	}
	if (!ext4_has_metadata_csum(inode->i_sb) ||
	    buffer_verified(bh))
		return bh;

	/*
	 * An empty leaf block can get mistaken for a index block; for
	 * this reason, we can only check the index checksum when the
	 * caller is sure it should be an index block.
	 */
	if (is_dx_block && type == INDEX) {
		if (ext4_dx_csum_verify(inode, dirent) &&
		    !ext4_simulate_fail(inode->i_sb, EXT4_SIM_DIRBLOCK_CRC))
			set_buffer_verified(bh);
		else {
			ext4_error_inode_err(inode, func, line, block,
					     EFSBADCRC,
					     "Directory index failed checksum");
			brelse(bh);
			return ERR_PTR(-EFSBADCRC);
		}
	}
	if (!is_dx_block) {
		if (ext4_dirblock_csum_verify(inode, bh) &&
		    !ext4_simulate_fail(inode->i_sb, EXT4_SIM_DIRBLOCK_CRC))
			set_buffer_verified(bh);
		else {
			ext4_error_inode_err(inode, func, line, block,
					     EFSBADCRC,
					     "Directory block failed checksum");
			brelse(bh);
			return ERR_PTR(-EFSBADCRC);
		}
	}
	return bh;
}