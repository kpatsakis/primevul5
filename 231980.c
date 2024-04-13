static void __warn_no_space_for_csum(struct inode *inode, const char *func,
				     unsigned int line)
{
	__ext4_warning_inode(inode, func, line,
		"No space for directory leaf checksum. Please run e2fsck -D.");
}