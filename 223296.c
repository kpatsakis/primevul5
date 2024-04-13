static void remap_page(struct page *page)
{
	int i;
	if (PageTransHuge(page)) {
		remove_migration_ptes(page, page, true);
	} else {
		for (i = 0; i < HPAGE_PMD_NR; i++)
			remove_migration_ptes(page + i, page + i, true);
	}
}