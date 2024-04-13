static int fuse_writepage_locked(struct page *page)
{
	struct address_space *mapping = page->mapping;
	struct inode *inode = mapping->host;
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_inode *fi = get_fuse_inode(inode);
	struct fuse_writepage_args *wpa;
	struct fuse_args_pages *ap;
	struct page *tmp_page;
	int error = -ENOMEM;

	set_page_writeback(page);

	wpa = fuse_writepage_args_alloc();
	if (!wpa)
		goto err;
	ap = &wpa->ia.ap;

	tmp_page = alloc_page(GFP_NOFS | __GFP_HIGHMEM);
	if (!tmp_page)
		goto err_free;

	error = -EIO;
	wpa->ia.ff = fuse_write_file_get(fc, fi);
	if (!wpa->ia.ff)
		goto err_nofile;

	fuse_write_args_fill(&wpa->ia, wpa->ia.ff, page_offset(page), 0);

	copy_highpage(tmp_page, page);
	wpa->ia.write.in.write_flags |= FUSE_WRITE_CACHE;
	wpa->next = NULL;
	ap->args.in_pages = true;
	ap->num_pages = 1;
	ap->pages[0] = tmp_page;
	ap->descs[0].offset = 0;
	ap->descs[0].length = PAGE_SIZE;
	ap->args.end = fuse_writepage_end;
	wpa->inode = inode;

	inc_wb_stat(&inode_to_bdi(inode)->wb, WB_WRITEBACK);
	inc_node_page_state(tmp_page, NR_WRITEBACK_TEMP);

	spin_lock(&fi->lock);
	tree_insert(&fi->writepages, wpa);
	list_add_tail(&wpa->queue_entry, &fi->queued_writes);
	fuse_flush_writepages(inode);
	spin_unlock(&fi->lock);

	end_page_writeback(page);

	return 0;

err_nofile:
	__free_page(tmp_page);
err_free:
	kfree(wpa);
err:
	mapping_set_error(page->mapping, error);
	end_page_writeback(page);
	return error;
}