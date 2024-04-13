static int fuse_readpages_fill(void *_data, struct page *page)
{
	struct fuse_fill_data *data = _data;
	struct fuse_req *req = data->req;
	struct inode *inode = data->inode;
	struct fuse_conn *fc = get_fuse_conn(inode);

	fuse_wait_on_page_writeback(inode, page->index);

	if (req->num_pages &&
	    (req->num_pages == FUSE_MAX_PAGES_PER_REQ ||
	     (req->num_pages + 1) * PAGE_CACHE_SIZE > fc->max_read ||
	     req->pages[req->num_pages - 1]->index + 1 != page->index)) {
		fuse_send_readpages(req, data->file);
		data->req = req = fuse_get_req(fc);
		if (IS_ERR(req)) {
			unlock_page(page);
			return PTR_ERR(req);
		}
	}
	req->pages[req->num_pages] = page;
	req->num_pages++;
	return 0;
}