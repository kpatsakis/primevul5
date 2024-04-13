static int fuse_readpage(struct file *file, struct page *page)
{
	struct inode *inode = page->mapping->host;
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_req *req;
	size_t num_read;
	loff_t pos = page_offset(page);
	size_t count = PAGE_CACHE_SIZE;
	u64 attr_ver;
	int err;

	err = -EIO;
	if (is_bad_inode(inode))
		goto out;

	/*
	 * Page writeback can extend beyond the liftime of the
	 * page-cache page, so make sure we read a properly synced
	 * page.
	 */
	fuse_wait_on_page_writeback(inode, page->index);

	req = fuse_get_req(fc);
	err = PTR_ERR(req);
	if (IS_ERR(req))
		goto out;

	attr_ver = fuse_get_attr_version(fc);

	req->out.page_zeroing = 1;
	req->out.argpages = 1;
	req->num_pages = 1;
	req->pages[0] = page;
	num_read = fuse_send_read(req, file, pos, count, NULL);
	err = req->out.h.error;
	fuse_put_request(fc, req);

	if (!err) {
		/*
		 * Short read means EOF.  If file size is larger, truncate it
		 */
		if (num_read < count)
			fuse_read_update_size(inode, pos + num_read, attr_ver);

		SetPageUptodate(page);
	}

	fuse_invalidate_attr(inode); /* atime changed */
 out:
	unlock_page(page);
	return err;
}