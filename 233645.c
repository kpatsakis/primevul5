static int ecryptfs_encrypt_extent(struct page *enc_extent_page,
				   struct ecryptfs_crypt_stat *crypt_stat,
				   struct page *page,
				   unsigned long extent_offset)
{
	loff_t extent_base;
	char extent_iv[ECRYPTFS_MAX_IV_BYTES];
	int rc;

	extent_base = (((loff_t)page->index)
		       * (PAGE_CACHE_SIZE / crypt_stat->extent_size));
	rc = ecryptfs_derive_iv(extent_iv, crypt_stat,
				(extent_base + extent_offset));
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error attempting to "
				"derive IV for extent [0x%.16x]; "
				"rc = [%d]\n", (extent_base + extent_offset),
				rc);
		goto out;
	}
	if (unlikely(ecryptfs_verbosity > 0)) {
		ecryptfs_printk(KERN_DEBUG, "Encrypting extent "
				"with iv:\n");
		ecryptfs_dump_hex(extent_iv, crypt_stat->iv_bytes);
		ecryptfs_printk(KERN_DEBUG, "First 8 bytes before "
				"encryption:\n");
		ecryptfs_dump_hex((char *)
				  (page_address(page)
				   + (extent_offset * crypt_stat->extent_size)),
				  8);
	}
	rc = ecryptfs_encrypt_page_offset(crypt_stat, enc_extent_page, 0,
					  page, (extent_offset
						 * crypt_stat->extent_size),
					  crypt_stat->extent_size, extent_iv);
	if (rc < 0) {
		printk(KERN_ERR "%s: Error attempting to encrypt page with "
		       "page->index = [%ld], extent_offset = [%ld]; "
		       "rc = [%d]\n", __func__, page->index, extent_offset,
		       rc);
		goto out;
	}
	rc = 0;
	if (unlikely(ecryptfs_verbosity > 0)) {
		ecryptfs_printk(KERN_DEBUG, "Encrypt extent [0x%.16x]; "
				"rc = [%d]\n", (extent_base + extent_offset),
				rc);
		ecryptfs_printk(KERN_DEBUG, "First 8 bytes after "
				"encryption:\n");
		ecryptfs_dump_hex((char *)(page_address(enc_extent_page)), 8);
	}
out:
	return rc;
}