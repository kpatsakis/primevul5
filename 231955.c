static struct inode *ext4_whiteout_for_rename(struct ext4_renament *ent,
					      int credits, handle_t **h)
{
	struct inode *wh;
	handle_t *handle;
	int retries = 0;

	/*
	 * for inode block, sb block, group summaries,
	 * and inode bitmap
	 */
	credits += (EXT4_MAXQUOTAS_TRANS_BLOCKS(ent->dir->i_sb) +
		    EXT4_XATTR_TRANS_BLOCKS + 4);
retry:
	wh = ext4_new_inode_start_handle(ent->dir, S_IFCHR | WHITEOUT_MODE,
					 &ent->dentry->d_name, 0, NULL,
					 EXT4_HT_DIR, credits);

	handle = ext4_journal_current_handle();
	if (IS_ERR(wh)) {
		if (handle)
			ext4_journal_stop(handle);
		if (PTR_ERR(wh) == -ENOSPC &&
		    ext4_should_retry_alloc(ent->dir->i_sb, &retries))
			goto retry;
	} else {
		*h = handle;
		init_special_inode(wh, wh->i_mode, WHITEOUT_DEV);
		wh->i_op = &ext4_special_inode_operations;
	}
	return wh;
}