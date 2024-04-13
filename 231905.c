static struct buffer_head *__ext4_find_entry(struct inode *dir,
					     struct ext4_filename *fname,
					     struct ext4_dir_entry_2 **res_dir,
					     int *inlined)
{
	struct super_block *sb;
	struct buffer_head *bh_use[NAMEI_RA_SIZE];
	struct buffer_head *bh, *ret = NULL;
	ext4_lblk_t start, block;
	const u8 *name = fname->usr_fname->name;
	size_t ra_max = 0;	/* Number of bh's in the readahead
				   buffer, bh_use[] */
	size_t ra_ptr = 0;	/* Current index into readahead
				   buffer */
	ext4_lblk_t  nblocks;
	int i, namelen, retval;

	*res_dir = NULL;
	sb = dir->i_sb;
	namelen = fname->usr_fname->len;
	if (namelen > EXT4_NAME_LEN)
		return NULL;

	if (ext4_has_inline_data(dir)) {
		int has_inline_data = 1;
		ret = ext4_find_inline_entry(dir, fname, res_dir,
					     &has_inline_data);
		if (has_inline_data) {
			if (inlined)
				*inlined = 1;
			goto cleanup_and_exit;
		}
	}

	if ((namelen <= 2) && (name[0] == '.') &&
	    (name[1] == '.' || name[1] == '\0')) {
		/*
		 * "." or ".." will only be in the first block
		 * NFS may look up ".."; "." should be handled by the VFS
		 */
		block = start = 0;
		nblocks = 1;
		goto restart;
	}
	if (is_dx(dir)) {
		ret = ext4_dx_find_entry(dir, fname, res_dir);
		/*
		 * On success, or if the error was file not found,
		 * return.  Otherwise, fall back to doing a search the
		 * old fashioned way.
		 */
		if (!IS_ERR(ret) || PTR_ERR(ret) != ERR_BAD_DX_DIR)
			goto cleanup_and_exit;
		dxtrace(printk(KERN_DEBUG "ext4_find_entry: dx failed, "
			       "falling back\n"));
		ret = NULL;
	}
	nblocks = dir->i_size >> EXT4_BLOCK_SIZE_BITS(sb);
	if (!nblocks) {
		ret = NULL;
		goto cleanup_and_exit;
	}
	start = EXT4_I(dir)->i_dir_start_lookup;
	if (start >= nblocks)
		start = 0;
	block = start;
restart:
	do {
		/*
		 * We deal with the read-ahead logic here.
		 */
		cond_resched();
		if (ra_ptr >= ra_max) {
			/* Refill the readahead buffer */
			ra_ptr = 0;
			if (block < start)
				ra_max = start - block;
			else
				ra_max = nblocks - block;
			ra_max = min(ra_max, ARRAY_SIZE(bh_use));
			retval = ext4_bread_batch(dir, block, ra_max,
						  false /* wait */, bh_use);
			if (retval) {
				ret = ERR_PTR(retval);
				ra_max = 0;
				goto cleanup_and_exit;
			}
		}
		if ((bh = bh_use[ra_ptr++]) == NULL)
			goto next;
		wait_on_buffer(bh);
		if (!buffer_uptodate(bh)) {
			EXT4_ERROR_INODE_ERR(dir, EIO,
					     "reading directory lblock %lu",
					     (unsigned long) block);
			brelse(bh);
			ret = ERR_PTR(-EIO);
			goto cleanup_and_exit;
		}
		if (!buffer_verified(bh) &&
		    !is_dx_internal_node(dir, block,
					 (struct ext4_dir_entry *)bh->b_data) &&
		    !ext4_dirblock_csum_verify(dir, bh)) {
			EXT4_ERROR_INODE_ERR(dir, EFSBADCRC,
					     "checksumming directory "
					     "block %lu", (unsigned long)block);
			brelse(bh);
			ret = ERR_PTR(-EFSBADCRC);
			goto cleanup_and_exit;
		}
		set_buffer_verified(bh);
		i = search_dirblock(bh, dir, fname,
			    block << EXT4_BLOCK_SIZE_BITS(sb), res_dir);
		if (i == 1) {
			EXT4_I(dir)->i_dir_start_lookup = block;
			ret = bh;
			goto cleanup_and_exit;
		} else {
			brelse(bh);
			if (i < 0)
				goto cleanup_and_exit;
		}
	next:
		if (++block >= nblocks)
			block = 0;
	} while (block != start);

	/*
	 * If the directory has grown while we were searching, then
	 * search the last part of the directory before giving up.
	 */
	block = nblocks;
	nblocks = dir->i_size >> EXT4_BLOCK_SIZE_BITS(sb);
	if (block < nblocks) {
		start = 0;
		goto restart;
	}

cleanup_and_exit:
	/* Clean up the read-ahead blocks */
	for (; ra_ptr < ra_max; ra_ptr++)
		brelse(bh_use[ra_ptr]);
	return ret;
}