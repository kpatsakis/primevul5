int migrate_page(struct address_space *mapping,
		struct page *newpage, struct page *page)
{
	int rc;

	BUG_ON(PageWriteback(page));	/* Writeback must be complete */

	rc = migrate_page_move_mapping(mapping, newpage, page);

	if (rc)
		return rc;

	migrate_page_copy(newpage, page);
	return 0;
}