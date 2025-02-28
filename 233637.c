ecryptfs_write_metadata_to_contents(struct dentry *ecryptfs_dentry,
				    char *virt, size_t virt_len)
{
	int rc;

	rc = ecryptfs_write_lower(ecryptfs_dentry->d_inode, virt,
				  0, virt_len);
	if (rc)
		printk(KERN_ERR "%s: Error attempting to write header "
		       "information to lower file; rc = [%d]\n", __func__,
		       rc);
	return rc;
}