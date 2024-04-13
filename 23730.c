static int fuse_writepage(struct page *page, struct writeback_control *wbc)
{
	int err;

	err = fuse_writepage_locked(page);
	unlock_page(page);

	return err;
}