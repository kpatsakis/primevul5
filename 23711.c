static int fuse_write_end(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned copied,
			struct page *page, void *fsdata)
{
	struct inode *inode = mapping->host;
	int res = 0;

	if (copied)
		res = fuse_buffered_write(file, inode, pos, copied, page);

	unlock_page(page);
	page_cache_release(page);
	return res;
}