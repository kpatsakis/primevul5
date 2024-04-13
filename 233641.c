static void ecryptfs_lower_offset_for_extent(loff_t *offset, loff_t extent_num,
					     struct ecryptfs_crypt_stat *crypt_stat)
{
	(*offset) = (crypt_stat->num_header_bytes_at_front
		     + (crypt_stat->extent_size * extent_num));
}