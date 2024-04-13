static int fuse_do_readpage(struct file *file, struct page *page)
{
	struct inode *inode = page->mapping->host;
	struct fuse_mount *fm = get_fuse_mount(inode);
	loff_t pos = page_offset(page);
	struct fuse_page_desc desc = { .length = PAGE_SIZE };
	struct fuse_io_args ia = {
		.ap.args.page_zeroing = true,
		.ap.args.out_pages = true,
		.ap.num_pages = 1,
		.ap.pages = &page,
		.ap.descs = &desc,
	};
	ssize_t res;
	u64 attr_ver;

	/*
	 * Page writeback can extend beyond the lifetime of the
	 * page-cache page, so make sure we read a properly synced
	 * page.
	 */
	fuse_wait_on_page_writeback(inode, page->index);

	attr_ver = fuse_get_attr_version(fm->fc);

	/* Don't overflow end offset */
	if (pos + (desc.length - 1) == LLONG_MAX)
		desc.length--;

	fuse_read_args_fill(&ia, file, pos, desc.length, FUSE_READ);
	res = fuse_simple_request(fm, &ia.ap.args);
	if (res < 0)
		return res;
	/*
	 * Short read means EOF.  If file size is larger, truncate it
	 */
	if (res < desc.length)
		fuse_short_read(inode, attr_ver, res, &ia.ap);

	SetPageUptodate(page);

	return 0;
}