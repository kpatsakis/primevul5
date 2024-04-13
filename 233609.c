int ecryptfs_read_and_validate_xattr_region(char *page_virt,
					    struct dentry *ecryptfs_dentry)
{
	int rc;

	rc = ecryptfs_read_xattr_region(page_virt, ecryptfs_dentry->d_inode);
	if (rc)
		goto out;
	if (!contains_ecryptfs_marker(page_virt	+ ECRYPTFS_FILE_SIZE_BYTES)) {
		printk(KERN_WARNING "Valid data found in [%s] xattr, but "
			"the marker is invalid\n", ECRYPTFS_XATTR_NAME);
		rc = -EINVAL;
	}
out:
	return rc;
}