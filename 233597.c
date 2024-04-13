int ecryptfs_decrypt_page(struct page *page)
{
	struct inode *ecryptfs_inode;
	struct ecryptfs_crypt_stat *crypt_stat;
	char *enc_extent_virt;
	struct page *enc_extent_page = NULL;
	unsigned long extent_offset;
	int rc = 0;

	ecryptfs_inode = page->mapping->host;
	crypt_stat =
		&(ecryptfs_inode_to_private(ecryptfs_inode)->crypt_stat);
	if (!(crypt_stat->flags & ECRYPTFS_ENCRYPTED)) {
		rc = ecryptfs_read_lower_page_segment(page, page->index, 0,
						      PAGE_CACHE_SIZE,
						      ecryptfs_inode);
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

		ecryptfs_lower_offset_for_extent(
			&offset, ((page->index * (PAGE_CACHE_SIZE
						  / crypt_stat->extent_size))
				  + extent_offset), crypt_stat);
		rc = ecryptfs_read_lower(enc_extent_virt, offset,
					 crypt_stat->extent_size,
					 ecryptfs_inode);
		if (rc) {
			ecryptfs_printk(KERN_ERR, "Error attempting "
					"to read lower page; rc = [%d]"
					"\n", rc);
			goto out;
		}
		rc = ecryptfs_decrypt_extent(page, crypt_stat, enc_extent_page,
					     extent_offset);
		if (rc) {
			printk(KERN_ERR "%s: Error encrypting extent; "
			       "rc = [%d]\n", __func__, rc);
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