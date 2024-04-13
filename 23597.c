static int fallback_migrate_page(struct address_space *mapping,
	struct page *newpage, struct page *page)
{
	if (PageDirty(page))
		return writeout(mapping, page);

	/*
	 * Buffers may be managed in a filesystem specific way.
	 * We must have no buffers or drop them.
	 */
	if (PagePrivate(page) &&
	    !try_to_release_page(page, GFP_KERNEL))
		return -EAGAIN;

	return migrate_page(mapping, newpage, page);
}