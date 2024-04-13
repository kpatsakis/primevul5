static void xennet_make_txreqs(
	struct xennet_gnttab_make_txreq *info,
	struct page *page,
	unsigned int offset, unsigned int len)
{
	/* Skip unused frames from start of page */
	page += offset >> PAGE_SHIFT;
	offset &= ~PAGE_MASK;

	while (len) {
		info->page = page;
		info->size = 0;

		gnttab_foreach_grant_in_range(page, offset, len,
					      xennet_make_one_txreq,
					      info);

		page++;
		offset = 0;
		len -= info->size;
	}
}