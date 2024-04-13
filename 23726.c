static int fuse_buffered_write(struct file *file, struct inode *inode,
			       loff_t pos, unsigned count, struct page *page)
{
	int err;
	size_t nres;
	struct fuse_conn *fc = get_fuse_conn(inode);
	unsigned offset = pos & (PAGE_CACHE_SIZE - 1);
	struct fuse_req *req;

	if (is_bad_inode(inode))
		return -EIO;

	/*
	 * Make sure writepages on the same page are not mixed up with
	 * plain writes.
	 */
	fuse_wait_on_page_writeback(inode, page->index);

	req = fuse_get_req(fc);
	if (IS_ERR(req))
		return PTR_ERR(req);

	req->in.argpages = 1;
	req->num_pages = 1;
	req->pages[0] = page;
	req->page_offset = offset;
	nres = fuse_send_write(req, file, pos, count, NULL);
	err = req->out.h.error;
	fuse_put_request(fc, req);
	if (!err && !nres)
		err = -EIO;
	if (!err) {
		pos += nres;
		fuse_write_update_size(inode, pos);
		if (count == PAGE_CACHE_SIZE)
			SetPageUptodate(page);
	}
	fuse_invalidate_attr(inode);
	return err ? err : nres;
}