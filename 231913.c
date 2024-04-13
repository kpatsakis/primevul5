static int ext4_dx_add_entry(handle_t *handle, struct ext4_filename *fname,
			     struct inode *dir, struct inode *inode)
{
	struct dx_frame frames[EXT4_HTREE_LEVEL], *frame;
	struct dx_entry *entries, *at;
	struct buffer_head *bh;
	struct super_block *sb = dir->i_sb;
	struct ext4_dir_entry_2 *de;
	int restart;
	int err;

again:
	restart = 0;
	frame = dx_probe(fname, dir, NULL, frames);
	if (IS_ERR(frame))
		return PTR_ERR(frame);
	entries = frame->entries;
	at = frame->at;
	bh = ext4_read_dirblock(dir, dx_get_block(frame->at), DIRENT_HTREE);
	if (IS_ERR(bh)) {
		err = PTR_ERR(bh);
		bh = NULL;
		goto cleanup;
	}

	BUFFER_TRACE(bh, "get_write_access");
	err = ext4_journal_get_write_access(handle, bh);
	if (err)
		goto journal_error;

	err = add_dirent_to_buf(handle, fname, dir, inode, NULL, bh);
	if (err != -ENOSPC)
		goto cleanup;

	err = 0;
	/* Block full, should compress but for now just split */
	dxtrace(printk(KERN_DEBUG "using %u of %u node entries\n",
		       dx_get_count(entries), dx_get_limit(entries)));
	/* Need to split index? */
	if (dx_get_count(entries) == dx_get_limit(entries)) {
		ext4_lblk_t newblock;
		int levels = frame - frames + 1;
		unsigned int icount;
		int add_level = 1;
		struct dx_entry *entries2;
		struct dx_node *node2;
		struct buffer_head *bh2;

		while (frame > frames) {
			if (dx_get_count((frame - 1)->entries) <
			    dx_get_limit((frame - 1)->entries)) {
				add_level = 0;
				break;
			}
			frame--; /* split higher index block */
			at = frame->at;
			entries = frame->entries;
			restart = 1;
		}
		if (add_level && levels == ext4_dir_htree_level(sb)) {
			ext4_warning(sb, "Directory (ino: %lu) index full, "
					 "reach max htree level :%d",
					 dir->i_ino, levels);
			if (ext4_dir_htree_level(sb) < EXT4_HTREE_LEVEL) {
				ext4_warning(sb, "Large directory feature is "
						 "not enabled on this "
						 "filesystem");
			}
			err = -ENOSPC;
			goto cleanup;
		}
		icount = dx_get_count(entries);
		bh2 = ext4_append(handle, dir, &newblock);
		if (IS_ERR(bh2)) {
			err = PTR_ERR(bh2);
			goto cleanup;
		}
		node2 = (struct dx_node *)(bh2->b_data);
		entries2 = node2->entries;
		memset(&node2->fake, 0, sizeof(struct fake_dirent));
		node2->fake.rec_len = ext4_rec_len_to_disk(sb->s_blocksize,
							   sb->s_blocksize);
		BUFFER_TRACE(frame->bh, "get_write_access");
		err = ext4_journal_get_write_access(handle, frame->bh);
		if (err)
			goto journal_error;
		if (!add_level) {
			unsigned icount1 = icount/2, icount2 = icount - icount1;
			unsigned hash2 = dx_get_hash(entries + icount1);
			dxtrace(printk(KERN_DEBUG "Split index %i/%i\n",
				       icount1, icount2));

			BUFFER_TRACE(frame->bh, "get_write_access"); /* index root */
			err = ext4_journal_get_write_access(handle,
							     (frame - 1)->bh);
			if (err)
				goto journal_error;

			memcpy((char *) entries2, (char *) (entries + icount1),
			       icount2 * sizeof(struct dx_entry));
			dx_set_count(entries, icount1);
			dx_set_count(entries2, icount2);
			dx_set_limit(entries2, dx_node_limit(dir));

			/* Which index block gets the new entry? */
			if (at - entries >= icount1) {
				frame->at = at = at - entries - icount1 + entries2;
				frame->entries = entries = entries2;
				swap(frame->bh, bh2);
			}
			dx_insert_block((frame - 1), hash2, newblock);
			dxtrace(dx_show_index("node", frame->entries));
			dxtrace(dx_show_index("node",
			       ((struct dx_node *) bh2->b_data)->entries));
			err = ext4_handle_dirty_dx_node(handle, dir, bh2);
			if (err)
				goto journal_error;
			brelse (bh2);
			err = ext4_handle_dirty_dx_node(handle, dir,
						   (frame - 1)->bh);
			if (err)
				goto journal_error;
			if (restart) {
				err = ext4_handle_dirty_dx_node(handle, dir,
							   frame->bh);
				goto journal_error;
			}
		} else {
			struct dx_root *dxroot;
			memcpy((char *) entries2, (char *) entries,
			       icount * sizeof(struct dx_entry));
			dx_set_limit(entries2, dx_node_limit(dir));

			/* Set up root */
			dx_set_count(entries, 1);
			dx_set_block(entries + 0, newblock);
			dxroot = (struct dx_root *)frames[0].bh->b_data;
			dxroot->info.indirect_levels += 1;
			dxtrace(printk(KERN_DEBUG
				       "Creating %d level index...\n",
				       dxroot->info.indirect_levels));
			err = ext4_handle_dirty_dx_node(handle, dir, frame->bh);
			if (err)
				goto journal_error;
			err = ext4_handle_dirty_dx_node(handle, dir, bh2);
			brelse(bh2);
			restart = 1;
			goto journal_error;
		}
	}
	de = do_split(handle, dir, &bh, frame, &fname->hinfo);
	if (IS_ERR(de)) {
		err = PTR_ERR(de);
		goto cleanup;
	}
	err = add_dirent_to_buf(handle, fname, dir, inode, de, bh);
	goto cleanup;

journal_error:
	ext4_std_error(dir->i_sb, err); /* this is a no-op if err == 0 */
cleanup:
	brelse(bh);
	dx_release(frames);
	/* @restart is true means htree-path has been changed, we need to
	 * repeat dx_probe() to find out valid htree-path
	 */
	if (restart && err == 0)
		goto again;
	return err;
}