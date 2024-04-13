static ssize_t fuse_fill_write_pages(struct fuse_args_pages *ap,
				     struct address_space *mapping,
				     struct iov_iter *ii, loff_t pos,
				     unsigned int max_pages)
{
	struct fuse_conn *fc = get_fuse_conn(mapping->host);
	unsigned offset = pos & (PAGE_SIZE - 1);
	size_t count = 0;
	int err;

	ap->args.in_pages = true;
	ap->descs[0].offset = offset;

	do {
		size_t tmp;
		struct page *page;
		pgoff_t index = pos >> PAGE_SHIFT;
		size_t bytes = min_t(size_t, PAGE_SIZE - offset,
				     iov_iter_count(ii));

		bytes = min_t(size_t, bytes, fc->max_write - count);

 again:
		err = -EFAULT;
		if (iov_iter_fault_in_readable(ii, bytes))
			break;

		err = -ENOMEM;
		page = grab_cache_page_write_begin(mapping, index, 0);
		if (!page)
			break;

		if (mapping_writably_mapped(mapping))
			flush_dcache_page(page);

		tmp = iov_iter_copy_from_user_atomic(page, ii, offset, bytes);
		flush_dcache_page(page);

		iov_iter_advance(ii, tmp);
		if (!tmp) {
			unlock_page(page);
			put_page(page);
			bytes = min(bytes, iov_iter_single_seg_count(ii));
			goto again;
		}

		err = 0;
		ap->pages[ap->num_pages] = page;
		ap->descs[ap->num_pages].length = tmp;
		ap->num_pages++;

		count += tmp;
		pos += tmp;
		offset += tmp;
		if (offset == PAGE_SIZE)
			offset = 0;

		if (!fc->big_writes)
			break;
	} while (iov_iter_count(ii) && count < fc->max_write &&
		 ap->num_pages < max_pages && offset == 0);

	return count > 0 ? count : err;
}