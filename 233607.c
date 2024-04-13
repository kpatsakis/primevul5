static void set_default_header_data(struct ecryptfs_crypt_stat *crypt_stat)
{
	crypt_stat->num_header_bytes_at_front =
		ECRYPTFS_MINIMUM_HEADER_EXTENT_SIZE;
}