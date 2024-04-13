int ext4_dirblock_csum_verify(struct inode *inode, struct buffer_head *bh)
{
	struct ext4_dir_entry_tail *t;

	if (!ext4_has_metadata_csum(inode->i_sb))
		return 1;

	t = get_dirent_tail(inode, bh);
	if (!t) {
		warn_no_space_for_csum(inode);
		return 0;
	}

	if (t->det_checksum != ext4_dirblock_csum(inode, bh->b_data,
						  (char *)t - bh->b_data))
		return 0;

	return 1;
}