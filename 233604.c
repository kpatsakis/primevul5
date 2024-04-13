int ecryptfs_read_and_validate_header_region(char *data,
					     struct inode *ecryptfs_inode)
{
	struct ecryptfs_crypt_stat *crypt_stat =
		&(ecryptfs_inode_to_private(ecryptfs_inode)->crypt_stat);
	int rc;

	if (crypt_stat->extent_size == 0)
		crypt_stat->extent_size = ECRYPTFS_DEFAULT_EXTENT_SIZE;
	rc = ecryptfs_read_lower(data, 0, crypt_stat->extent_size,
				 ecryptfs_inode);
	if (rc) {
		printk(KERN_ERR "%s: Error reading header region; rc = [%d]\n",
		       __func__, rc);
		goto out;
	}
	if (!contains_ecryptfs_marker(data + ECRYPTFS_FILE_SIZE_BYTES)) {
		rc = -EINVAL;
	}
out:
	return rc;
}