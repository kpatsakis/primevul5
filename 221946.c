static bool fuse_writepage_need_send(struct fuse_conn *fc, struct page *page,
				     struct fuse_args_pages *ap,
				     struct fuse_fill_wb_data *data)
{
	WARN_ON(!ap->num_pages);

	/*
	 * Being under writeback is unlikely but possible.  For example direct
	 * read to an mmaped fuse file will set the page dirty twice; once when
	 * the pages are faulted with get_user_pages(), and then after the read
	 * completed.
	 */
	if (fuse_page_is_writeback(data->inode, page->index))
		return true;

	/* Reached max pages */
	if (ap->num_pages == fc->max_pages)
		return true;

	/* Reached max write bytes */
	if ((ap->num_pages + 1) * PAGE_SIZE > fc->max_write)
		return true;

	/* Discontinuity */
	if (data->orig_pages[ap->num_pages - 1]->index + 1 != page->index)
		return true;

	/* Need to grow the pages array?  If so, did the expansion fail? */
	if (ap->num_pages == data->max_pages && !fuse_pages_realloc(data))
		return true;

	return false;
}