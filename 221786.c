static bool fuse_pages_realloc(struct fuse_fill_wb_data *data)
{
	struct fuse_args_pages *ap = &data->wpa->ia.ap;
	struct fuse_conn *fc = get_fuse_conn(data->inode);
	struct page **pages;
	struct fuse_page_desc *descs;
	unsigned int npages = min_t(unsigned int,
				    max_t(unsigned int, data->max_pages * 2,
					  FUSE_DEFAULT_MAX_PAGES_PER_REQ),
				    fc->max_pages);
	WARN_ON(npages <= data->max_pages);

	pages = fuse_pages_alloc(npages, GFP_NOFS, &descs);
	if (!pages)
		return false;

	memcpy(pages, ap->pages, sizeof(struct page *) * ap->num_pages);
	memcpy(descs, ap->descs, sizeof(struct fuse_page_desc) * ap->num_pages);
	kfree(ap->pages);
	ap->pages = pages;
	ap->descs = descs;
	data->max_pages = npages;

	return true;
}