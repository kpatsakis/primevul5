static int fuse_writepage_locked(struct page *page)
{
	struct address_space *mapping = page->mapping;
	struct inode *inode = mapping->host;
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_inode *fi = get_fuse_inode(inode);
	struct fuse_req *req;
	struct fuse_file *ff;
	struct page *tmp_page;

	set_page_writeback(page);

	req = fuse_request_alloc_nofs();
	if (!req)
		goto err;

	tmp_page = alloc_page(GFP_NOFS | __GFP_HIGHMEM);
	if (!tmp_page)
		goto err_free;

	spin_lock(&fc->lock);
	BUG_ON(list_empty(&fi->write_files));
	ff = list_entry(fi->write_files.next, struct fuse_file, write_entry);
	req->ff = fuse_file_get(ff);
	spin_unlock(&fc->lock);

	fuse_write_fill(req, ff, page_offset(page), 0);

	copy_highpage(tmp_page, page);
	req->misc.write.in.write_flags |= FUSE_WRITE_CACHE;
	req->in.argpages = 1;
	req->num_pages = 1;
	req->pages[0] = tmp_page;
	req->page_offset = 0;
	req->end = fuse_writepage_end;
	req->inode = inode;

	inc_bdi_stat(mapping->backing_dev_info, BDI_WRITEBACK);
	inc_zone_page_state(tmp_page, NR_WRITEBACK_TEMP);
	end_page_writeback(page);

	spin_lock(&fc->lock);
	list_add(&req->writepages_entry, &fi->writepages);
	list_add_tail(&req->list, &fi->queued_writes);
	fuse_flush_writepages(inode);
	spin_unlock(&fc->lock);

	return 0;

err_free:
	fuse_request_free(req);
err:
	end_page_writeback(page);
	return -ENOMEM;
}