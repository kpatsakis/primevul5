int ext4_group_extend(struct super_block *sb, struct ext4_super_block *es,
		      ext4_fsblk_t n_blocks_count)
{
	ext4_fsblk_t o_blocks_count;
	ext4_group_t o_groups_count;
	ext4_grpblk_t last;
	ext4_grpblk_t add;
	struct buffer_head *bh;
	handle_t *handle;
	int err;
	ext4_group_t group;

	/* We don't need to worry about locking wrt other resizers just
	 * yet: we're going to revalidate es->s_blocks_count after
	 * taking lock_super() below. */
	o_blocks_count = ext4_blocks_count(es);
	o_groups_count = EXT4_SB(sb)->s_groups_count;

	if (test_opt(sb, DEBUG))
		printk(KERN_DEBUG "EXT4-fs: extending last group from %llu uto %llu blocks\n",
		       o_blocks_count, n_blocks_count);

	if (n_blocks_count == 0 || n_blocks_count == o_blocks_count)
		return 0;

	if (n_blocks_count > (sector_t)(~0ULL) >> (sb->s_blocksize_bits - 9)) {
		printk(KERN_ERR "EXT4-fs: filesystem on %s:"
			" too large to resize to %llu blocks safely\n",
			sb->s_id, n_blocks_count);
		if (sizeof(sector_t) < 8)
			ext4_warning(sb, __func__, "CONFIG_LBD not enabled");
		return -EINVAL;
	}

	if (n_blocks_count < o_blocks_count) {
		ext4_warning(sb, __func__,
			     "can't shrink FS - resize aborted");
		return -EBUSY;
	}

	/* Handle the remaining blocks in the last group only. */
	ext4_get_group_no_and_offset(sb, o_blocks_count, &group, &last);

	if (last == 0) {
		ext4_warning(sb, __func__,
			     "need to use ext2online to resize further");
		return -EPERM;
	}

	add = EXT4_BLOCKS_PER_GROUP(sb) - last;

	if (o_blocks_count + add < o_blocks_count) {
		ext4_warning(sb, __func__, "blocks_count overflow");
		return -EINVAL;
	}

	if (o_blocks_count + add > n_blocks_count)
		add = n_blocks_count - o_blocks_count;

	if (o_blocks_count + add < n_blocks_count)
		ext4_warning(sb, __func__,
			     "will only finish group (%llu"
			     " blocks, %u new)",
			     o_blocks_count + add, add);

	/* See if the device is actually as big as what was requested */
	bh = sb_bread(sb, o_blocks_count + add - 1);
	if (!bh) {
		ext4_warning(sb, __func__,
			     "can't read last block, resize aborted");
		return -ENOSPC;
	}
	brelse(bh);

	/* We will update the superblock, one block bitmap, and
	 * one group descriptor via ext4_free_blocks().
	 */
	handle = ext4_journal_start_sb(sb, 3);
	if (IS_ERR(handle)) {
		err = PTR_ERR(handle);
		ext4_warning(sb, __func__, "error %d on journal start", err);
		goto exit_put;
	}

	lock_super(sb);
	if (o_blocks_count != ext4_blocks_count(es)) {
		ext4_warning(sb, __func__,
			     "multiple resizers run on filesystem!");
		unlock_super(sb);
		ext4_journal_stop(handle);
		err = -EBUSY;
		goto exit_put;
	}

	if ((err = ext4_journal_get_write_access(handle,
						 EXT4_SB(sb)->s_sbh))) {
		ext4_warning(sb, __func__,
			     "error %d on journal write access", err);
		unlock_super(sb);
		ext4_journal_stop(handle);
		goto exit_put;
	}
	ext4_blocks_count_set(es, o_blocks_count + add);
	ext4_handle_dirty_metadata(handle, NULL, EXT4_SB(sb)->s_sbh);
	sb->s_dirt = 1;
	unlock_super(sb);
	ext4_debug("freeing blocks %llu through %llu\n", o_blocks_count,
		   o_blocks_count + add);
	/* We add the blocks to the bitmap and set the group need init bit */
	ext4_add_groupblocks(handle, sb, o_blocks_count, add);
	ext4_debug("freed blocks %llu through %llu\n", o_blocks_count,
		   o_blocks_count + add);
	if ((err = ext4_journal_stop(handle)))
		goto exit_put;

	if (test_opt(sb, DEBUG))
		printk(KERN_DEBUG "EXT4-fs: extended group to %llu blocks\n",
		       ext4_blocks_count(es));
	update_backups(sb, EXT4_SB(sb)->s_sbh->b_blocknr, (char *)es,
		       sizeof(struct ext4_super_block));
exit_put:
	return err;
} /* ext4_group_extend */