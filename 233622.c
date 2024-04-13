void ecryptfs_set_default_sizes(struct ecryptfs_crypt_stat *crypt_stat)
{
	/* Default values; may be overwritten as we are parsing the
	 * packets. */
	crypt_stat->extent_size = ECRYPTFS_DEFAULT_EXTENT_SIZE;
	set_extent_mask_and_shift(crypt_stat);
	crypt_stat->iv_bytes = ECRYPTFS_DEFAULT_IV_BYTES;
	if (crypt_stat->flags & ECRYPTFS_METADATA_IN_XATTR)
		crypt_stat->num_header_bytes_at_front = 0;
	else {
		if (PAGE_CACHE_SIZE <= ECRYPTFS_MINIMUM_HEADER_EXTENT_SIZE)
			crypt_stat->num_header_bytes_at_front =
				ECRYPTFS_MINIMUM_HEADER_EXTENT_SIZE;
		else
			crypt_stat->num_header_bytes_at_front =	PAGE_CACHE_SIZE;
	}
}