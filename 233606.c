static int ecryptfs_write_headers_virt(char *page_virt, size_t max,
				       size_t *size,
				       struct ecryptfs_crypt_stat *crypt_stat,
				       struct dentry *ecryptfs_dentry)
{
	int rc;
	size_t written;
	size_t offset;

	offset = ECRYPTFS_FILE_SIZE_BYTES;
	write_ecryptfs_marker((page_virt + offset), &written);
	offset += written;
	write_ecryptfs_flags((page_virt + offset), crypt_stat, &written);
	offset += written;
	ecryptfs_write_header_metadata((page_virt + offset), crypt_stat,
				       &written);
	offset += written;
	rc = ecryptfs_generate_key_packet_set((page_virt + offset), crypt_stat,
					      ecryptfs_dentry, &written,
					      max - offset);
	if (rc)
		ecryptfs_printk(KERN_WARNING, "Error generating key packet "
				"set; rc = [%d]\n", rc);
	if (size) {
		offset += written;
		*size = offset;
	}
	return rc;
}