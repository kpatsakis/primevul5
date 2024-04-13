int total_mapcount(struct page *page)
{
	int i, compound, ret;

	VM_BUG_ON_PAGE(PageTail(page), page);

	if (likely(!PageCompound(page)))
		return atomic_read(&page->_mapcount) + 1;

	compound = compound_mapcount(page);
	if (PageHuge(page))
		return compound;
	ret = compound;
	for (i = 0; i < HPAGE_PMD_NR; i++)
		ret += atomic_read(&page[i]._mapcount) + 1;
	/* File pages has compound_mapcount included in _mapcount */
	if (!PageAnon(page))
		return ret - compound * HPAGE_PMD_NR;
	if (PageDoubleMap(page))
		ret -= HPAGE_PMD_NR;
	return ret;
}