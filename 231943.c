struct dentry *ext4_get_parent(struct dentry *child)
{
	__u32 ino;
	static const struct qstr dotdot = QSTR_INIT("..", 2);
	struct ext4_dir_entry_2 * de;
	struct buffer_head *bh;

	bh = ext4_find_entry(d_inode(child), &dotdot, &de, NULL);
	if (IS_ERR(bh))
		return ERR_CAST(bh);
	if (!bh)
		return ERR_PTR(-ENOENT);
	ino = le32_to_cpu(de->inode);
	brelse(bh);

	if (!ext4_valid_inum(child->d_sb, ino)) {
		EXT4_ERROR_INODE(d_inode(child),
				 "bad parent inode number: %u", ino);
		return ERR_PTR(-EFSCORRUPTED);
	}

	return d_obtain_alias(ext4_iget(child->d_sb, ino, EXT4_IGET_NORMAL));
}