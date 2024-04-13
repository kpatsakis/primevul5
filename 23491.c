static int move_to_new_page(struct page *newpage, struct page *page)
{
	struct address_space *mapping;
	int rc;

	/*
	 * Block others from accessing the page when we get around to
	 * establishing additional references. We are the only one
	 * holding a reference to the new page at this point.
	 */
	if (TestSetPageLocked(newpage))
		BUG();

	/* Prepare mapping for the new page.*/
	newpage->index = page->index;
	newpage->mapping = page->mapping;

	mapping = page_mapping(page);
	if (!mapping)
		rc = migrate_page(mapping, newpage, page);
	else if (mapping->a_ops->migratepage)
		/*
		 * Most pages have a mapping and most filesystems
		 * should provide a migration function. Anonymous
		 * pages are part of swap space which also has its
		 * own migration function. This is the most common
		 * path for page migration.
		 */
		rc = mapping->a_ops->migratepage(mapping,
						newpage, page);
	else
		rc = fallback_migrate_page(mapping, newpage, page);

	if (!rc) {
		mem_cgroup_page_migration(page, newpage);
		remove_migration_ptes(page, newpage);
	} else
		newpage->mapping = NULL;

	unlock_page(newpage);

	return rc;
}