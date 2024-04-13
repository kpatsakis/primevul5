static void ext4_dirblock_csum_set(struct inode *inode,
				 struct buffer_head *bh)
{
	struct ext4_dir_entry_tail *t;

	if (!ext4_has_metadata_csum(inode->i_sb))
		return;

	t = get_dirent_tail(inode, bh);
	if (!t) {
		warn_no_space_for_csum(inode);
		return;
	}

	t->det_checksum = ext4_dirblock_csum(inode, bh->b_data,
					     (char *)t - bh->b_data);
}