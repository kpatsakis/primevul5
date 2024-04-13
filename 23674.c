static void fuse_readpages_end(struct fuse_conn *fc, struct fuse_req *req)
{
	int i;
	size_t count = req->misc.read.in.size;
	size_t num_read = req->out.args[0].size;
	struct inode *inode = req->pages[0]->mapping->host;

	/*
	 * Short read means EOF.  If file size is larger, truncate it
	 */
	if (!req->out.h.error && num_read < count) {
		loff_t pos = page_offset(req->pages[0]) + num_read;
		fuse_read_update_size(inode, pos, req->misc.read.attr_ver);
	}

	fuse_invalidate_attr(inode); /* atime changed */

	for (i = 0; i < req->num_pages; i++) {
		struct page *page = req->pages[i];
		if (!req->out.h.error)
			SetPageUptodate(page);
		else
			SetPageError(page);
		unlock_page(page);
	}
	if (req->ff)
		fuse_file_put(req->ff);
}