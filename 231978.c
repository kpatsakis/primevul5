static struct buffer_head * ext4_dx_find_entry(struct inode *dir,
			struct ext4_filename *fname,
			struct ext4_dir_entry_2 **res_dir)
{
	struct super_block * sb = dir->i_sb;
	struct dx_frame frames[EXT4_HTREE_LEVEL], *frame;
	struct buffer_head *bh;
	ext4_lblk_t block;
	int retval;

#ifdef CONFIG_FS_ENCRYPTION
	*res_dir = NULL;
#endif
	frame = dx_probe(fname, dir, NULL, frames);
	if (IS_ERR(frame))
		return (struct buffer_head *) frame;
	do {
		block = dx_get_block(frame->at);
		bh = ext4_read_dirblock(dir, block, DIRENT_HTREE);
		if (IS_ERR(bh))
			goto errout;

		retval = search_dirblock(bh, dir, fname,
					 block << EXT4_BLOCK_SIZE_BITS(sb),
					 res_dir);
		if (retval == 1)
			goto success;
		brelse(bh);
		if (retval == -1) {
			bh = ERR_PTR(ERR_BAD_DX_DIR);
			goto errout;
		}

		/* Check to see if we should continue to search */
		retval = ext4_htree_next_block(dir, fname->hinfo.hash, frame,
					       frames, NULL);
		if (retval < 0) {
			ext4_warning_inode(dir,
				"error %d reading directory index block",
				retval);
			bh = ERR_PTR(retval);
			goto errout;
		}
	} while (retval == 1);

	bh = NULL;
errout:
	dxtrace(printk(KERN_DEBUG "%s not found\n", fname->usr_fname->name));
success:
	dx_release(frames);
	return bh;
}