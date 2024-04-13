static int add_new_gdb(handle_t *handle, struct inode *inode,
		       struct ext4_new_group_data *input,
		       struct buffer_head **primary)
{
	struct super_block *sb = inode->i_sb;
	struct ext4_super_block *es = EXT4_SB(sb)->s_es;
	unsigned long gdb_num = input->group / EXT4_DESC_PER_BLOCK(sb);
	ext4_fsblk_t gdblock = EXT4_SB(sb)->s_sbh->b_blocknr + 1 + gdb_num;
	struct buffer_head **o_group_desc, **n_group_desc;
	struct buffer_head *dind;
	int gdbackups;
	struct ext4_iloc iloc;
	__le32 *data;
	int err;

	if (test_opt(sb, DEBUG))
		printk(KERN_DEBUG
		       "EXT4-fs: ext4_add_new_gdb: adding group block %lu\n",
		       gdb_num);

	/*
	 * If we are not using the primary superblock/GDT copy don't resize,
         * because the user tools have no way of handling this.  Probably a
         * bad time to do it anyways.
         */
	if (EXT4_SB(sb)->s_sbh->b_blocknr !=
	    le32_to_cpu(EXT4_SB(sb)->s_es->s_first_data_block)) {
		ext4_warning(sb, __func__,
			"won't resize using backup superblock at %llu",
			(unsigned long long)EXT4_SB(sb)->s_sbh->b_blocknr);
		return -EPERM;
	}

	*primary = sb_bread(sb, gdblock);
	if (!*primary)
		return -EIO;

	if ((gdbackups = verify_reserved_gdb(sb, *primary)) < 0) {
		err = gdbackups;
		goto exit_bh;
	}

	data = EXT4_I(inode)->i_data + EXT4_DIND_BLOCK;
	dind = sb_bread(sb, le32_to_cpu(*data));
	if (!dind) {
		err = -EIO;
		goto exit_bh;
	}

	data = (__le32 *)dind->b_data;
	if (le32_to_cpu(data[gdb_num % EXT4_ADDR_PER_BLOCK(sb)]) != gdblock) {
		ext4_warning(sb, __func__,
			     "new group %u GDT block %llu not reserved",
			     input->group, gdblock);
		err = -EINVAL;
		goto exit_dind;
	}

	if ((err = ext4_journal_get_write_access(handle, EXT4_SB(sb)->s_sbh)))
		goto exit_dind;

	if ((err = ext4_journal_get_write_access(handle, *primary)))
		goto exit_sbh;

	if ((err = ext4_journal_get_write_access(handle, dind)))
		goto exit_primary;

	/* ext4_reserve_inode_write() gets a reference on the iloc */
	if ((err = ext4_reserve_inode_write(handle, inode, &iloc)))
		goto exit_dindj;

	n_group_desc = kmalloc((gdb_num + 1) * sizeof(struct buffer_head *),
			GFP_NOFS);
	if (!n_group_desc) {
		err = -ENOMEM;
		ext4_warning(sb, __func__,
			      "not enough memory for %lu groups", gdb_num + 1);
		goto exit_inode;
	}

	/*
	 * Finally, we have all of the possible failures behind us...
	 *
	 * Remove new GDT block from inode double-indirect block and clear out
	 * the new GDT block for use (which also "frees" the backup GDT blocks
	 * from the reserved inode).  We don't need to change the bitmaps for
	 * these blocks, because they are marked as in-use from being in the
	 * reserved inode, and will become GDT blocks (primary and backup).
	 */
	data[gdb_num % EXT4_ADDR_PER_BLOCK(sb)] = 0;
	ext4_handle_dirty_metadata(handle, NULL, dind);
	brelse(dind);
	inode->i_blocks -= (gdbackups + 1) * sb->s_blocksize >> 9;
	ext4_mark_iloc_dirty(handle, inode, &iloc);
	memset((*primary)->b_data, 0, sb->s_blocksize);
	ext4_handle_dirty_metadata(handle, NULL, *primary);

	o_group_desc = EXT4_SB(sb)->s_group_desc;
	memcpy(n_group_desc, o_group_desc,
	       EXT4_SB(sb)->s_gdb_count * sizeof(struct buffer_head *));
	n_group_desc[gdb_num] = *primary;
	EXT4_SB(sb)->s_group_desc = n_group_desc;
	EXT4_SB(sb)->s_gdb_count++;
	kfree(o_group_desc);

	le16_add_cpu(&es->s_reserved_gdt_blocks, -1);
	ext4_handle_dirty_metadata(handle, NULL, EXT4_SB(sb)->s_sbh);

	return 0;

exit_inode:
	/* ext4_journal_release_buffer(handle, iloc.bh); */
	brelse(iloc.bh);
exit_dindj:
	/* ext4_journal_release_buffer(handle, dind); */
exit_primary:
	/* ext4_journal_release_buffer(handle, *primary); */
exit_sbh:
	/* ext4_journal_release_buffer(handle, *primary); */
exit_dind:
	brelse(dind);
exit_bh:
	brelse(*primary);

	ext4_debug("leaving with error %d\n", err);
	return err;
}