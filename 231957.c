static int ext4_dx_csum_verify(struct inode *inode,
			       struct ext4_dir_entry *dirent)
{
	struct dx_countlimit *c;
	struct dx_tail *t;
	int count_offset, limit, count;

	if (!ext4_has_metadata_csum(inode->i_sb))
		return 1;

	c = get_dx_countlimit(inode, dirent, &count_offset);
	if (!c) {
		EXT4_ERROR_INODE(inode, "dir seems corrupt?  Run e2fsck -D.");
		return 0;
	}
	limit = le16_to_cpu(c->limit);
	count = le16_to_cpu(c->count);
	if (count_offset + (limit * sizeof(struct dx_entry)) >
	    EXT4_BLOCK_SIZE(inode->i_sb) - sizeof(struct dx_tail)) {
		warn_no_space_for_csum(inode);
		return 0;
	}
	t = (struct dx_tail *)(((struct dx_entry *)c) + limit);

	if (t->dt_checksum != ext4_dx_csum(inode, dirent, count_offset,
					    count, t))
		return 0;
	return 1;
}