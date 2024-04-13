static int setup_new_group_blocks(struct super_block *sb,
				  struct ext4_new_group_data *input)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	ext4_fsblk_t start = ext4_group_first_block_no(sb, input->group);
	int reserved_gdb = ext4_bg_has_super(sb, input->group) ?
		le16_to_cpu(sbi->s_es->s_reserved_gdt_blocks) : 0;
	unsigned long gdblocks = ext4_bg_num_gdb(sb, input->group);
	struct buffer_head *bh;
	handle_t *handle;
	ext4_fsblk_t block;
	ext4_grpblk_t bit;
	int i;
	int err = 0, err2;

	/* This transaction may be extended/restarted along the way */
	handle = ext4_journal_start_sb(sb, EXT4_MAX_TRANS_DATA);

	if (IS_ERR(handle))
		return PTR_ERR(handle);

	lock_super(sb);
	if (input->group != sbi->s_groups_count) {
		err = -EBUSY;
		goto exit_journal;
	}

	if (IS_ERR(bh = bclean(handle, sb, input->block_bitmap))) {
		err = PTR_ERR(bh);
		goto exit_journal;
	}

	if (ext4_bg_has_super(sb, input->group)) {
		ext4_debug("mark backup superblock %#04llx (+0)\n", start);
		ext4_set_bit(0, bh->b_data);
	}

	/* Copy all of the GDT blocks into the backup in this group */
	for (i = 0, bit = 1, block = start + 1;
	     i < gdblocks; i++, block++, bit++) {
		struct buffer_head *gdb;

		ext4_debug("update backup group %#04llx (+%d)\n", block, bit);

		if ((err = extend_or_restart_transaction(handle, 1, bh)))
			goto exit_bh;

		gdb = sb_getblk(sb, block);
		if (!gdb) {
			err = -EIO;
			goto exit_bh;
		}
		if ((err = ext4_journal_get_write_access(handle, gdb))) {
			brelse(gdb);
			goto exit_bh;
		}
		lock_buffer(gdb);
		memcpy(gdb->b_data, sbi->s_group_desc[i]->b_data, gdb->b_size);
		set_buffer_uptodate(gdb);
		unlock_buffer(gdb);
		ext4_handle_dirty_metadata(handle, NULL, gdb);
		ext4_set_bit(bit, bh->b_data);
		brelse(gdb);
	}

	/* Zero out all of the reserved backup group descriptor table blocks */
	for (i = 0, bit = gdblocks + 1, block = start + bit;
	     i < reserved_gdb; i++, block++, bit++) {
		struct buffer_head *gdb;

		ext4_debug("clear reserved block %#04llx (+%d)\n", block, bit);

		if ((err = extend_or_restart_transaction(handle, 1, bh)))
			goto exit_bh;

		if (IS_ERR(gdb = bclean(handle, sb, block))) {
			err = PTR_ERR(bh);
			goto exit_bh;
		}
		ext4_handle_dirty_metadata(handle, NULL, gdb);
		ext4_set_bit(bit, bh->b_data);
		brelse(gdb);
	}
	ext4_debug("mark block bitmap %#04llx (+%llu)\n", input->block_bitmap,
		   input->block_bitmap - start);
	ext4_set_bit(input->block_bitmap - start, bh->b_data);
	ext4_debug("mark inode bitmap %#04llx (+%llu)\n", input->inode_bitmap,
		   input->inode_bitmap - start);
	ext4_set_bit(input->inode_bitmap - start, bh->b_data);

	/* Zero out all of the inode table blocks */
	for (i = 0, block = input->inode_table, bit = block - start;
	     i < sbi->s_itb_per_group; i++, bit++, block++) {
		struct buffer_head *it;

		ext4_debug("clear inode block %#04llx (+%d)\n", block, bit);

		if ((err = extend_or_restart_transaction(handle, 1, bh)))
			goto exit_bh;

		if (IS_ERR(it = bclean(handle, sb, block))) {
			err = PTR_ERR(it);
			goto exit_bh;
		}
		ext4_handle_dirty_metadata(handle, NULL, it);
		brelse(it);
		ext4_set_bit(bit, bh->b_data);
	}

	if ((err = extend_or_restart_transaction(handle, 2, bh)))
		goto exit_bh;

	mark_bitmap_end(input->blocks_count, sb->s_blocksize * 8, bh->b_data);
	ext4_handle_dirty_metadata(handle, NULL, bh);
	brelse(bh);
	/* Mark unused entries in inode bitmap used */
	ext4_debug("clear inode bitmap %#04llx (+%llu)\n",
		   input->inode_bitmap, input->inode_bitmap - start);
	if (IS_ERR(bh = bclean(handle, sb, input->inode_bitmap))) {
		err = PTR_ERR(bh);
		goto exit_journal;
	}

	mark_bitmap_end(EXT4_INODES_PER_GROUP(sb), sb->s_blocksize * 8,
			bh->b_data);
	ext4_handle_dirty_metadata(handle, NULL, bh);
exit_bh:
	brelse(bh);

exit_journal:
	unlock_super(sb);
	if ((err2 = ext4_journal_stop(handle)) && !err)
		err = err2;

	return err;
}