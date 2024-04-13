int ecryptfs_encrypt_page(struct page *page)
{
	struct inode *ecryptfs_inode;
	struct ecryptfs_crypt_stat *crypt_stat;
	char *enc_extent_virt;
	struct page *enc_extent_page = NULL;
	loff_t extent_offset;
	int rc = 0;

	ecryptfs_inode = page->mapping->host;
	crypt_stat =
		&(ecryptfs_inode_to_private(ecryptfs_inode)->crypt_stat);
	if (!(crypt_stat->flags & ECRYPTFS_ENCRYPTED)) {
		rc = ecryptfs_write_lower_page_segment(ecryptfs_inode, page,
						       0, PAGE_CACHE_SIZE);
		if (rc)
			printk(KERN_ERR "%s: Error attempting to copy "
			       "page at index [%ld]\n", __func__,
			       page->index);
		goto out;
	}
	enc_extent_page = alloc_page(GFP_USER);
	if (!enc_extent_page) {
		rc = -ENOMEM;
		ecryptfs_printk(KERN_ERR, "Error allocating memory for "
				"encrypted extent\n");
		goto out;
	}
	enc_extent_virt = kmap(enc_extent_page);
	for (extent_offset = 0;
	     extent_offset < (PAGE_CACHE_SIZE / crypt_stat->extent_size);
	     extent_offset++) {
		loff_t offset;

		rc = ecryptfs_encrypt_extent(enc_extent_page, crypt_stat, page,
					     extent_offset);
		if (rc) {
			printk(KERN_ERR "%s: Error encrypting extent; "
			       "rc = [%d]\n", __func__, rc);
			goto out;
		}
		ecryptfs_lower_offset_for_extent(
			&offset, ((((loff_t)page->index)
				   * (PAGE_CACHE_SIZE
				      / crypt_stat->extent_size))
				  + extent_offset), crypt_stat);
		rc = ecryptfs_write_lower(ecryptfs_inode, enc_extent_virt,
					  offset, crypt_stat->extent_size);
		if (rc) {
			ecryptfs_printk(KERN_ERR, "Error attempting "
					"to write lower page; rc = [%d]"
					"\n", rc);
			goto out;
		}
	}
out:
	if (enc_extent_page) {
		kunmap(enc_extent_page);
		__free_page(enc_extent_page);
	}
	return rc;
}