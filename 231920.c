static struct dx_countlimit *get_dx_countlimit(struct inode *inode,
					       struct ext4_dir_entry *dirent,
					       int *offset)
{
	struct ext4_dir_entry *dp;
	struct dx_root_info *root;
	int count_offset;

	if (le16_to_cpu(dirent->rec_len) == EXT4_BLOCK_SIZE(inode->i_sb))
		count_offset = 8;
	else if (le16_to_cpu(dirent->rec_len) == 12) {
		dp = (struct ext4_dir_entry *)(((void *)dirent) + 12);
		if (le16_to_cpu(dp->rec_len) !=
		    EXT4_BLOCK_SIZE(inode->i_sb) - 12)
			return NULL;
		root = (struct dx_root_info *)(((void *)dp + 12));
		if (root->reserved_zero ||
		    root->info_length != sizeof(struct dx_root_info))
			return NULL;
		count_offset = 32;
	} else
		return NULL;

	if (offset)
		*offset = count_offset;
	return (struct dx_countlimit *)(((void *)dirent) + count_offset);
}