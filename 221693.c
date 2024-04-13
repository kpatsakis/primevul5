static int fuse_symlink_readpage(struct file *null, struct page *page)
{
	int err = fuse_readlink_page(page->mapping->host, page);

	if (!err)
		SetPageUptodate(page);

	unlock_page(page);

	return err;
}