static int fuse_readpage(struct file *file, struct page *page)
{
	struct inode *inode = page->mapping->host;
	int err;

	err = -EIO;
	if (fuse_is_bad(inode))
		goto out;

	err = fuse_do_readpage(file, page);
	fuse_invalidate_atime(inode);
 out:
	unlock_page(page);
	return err;
}