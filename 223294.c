bool can_split_huge_page(struct page *page, int *pextra_pins)
{
	int extra_pins;

	/* Additional pins from page cache */
	if (PageAnon(page))
		extra_pins = PageSwapCache(page) ? HPAGE_PMD_NR : 0;
	else
		extra_pins = HPAGE_PMD_NR;
	if (pextra_pins)
		*pextra_pins = extra_pins;
	return total_mapcount(page) == page_count(page) - extra_pins - 1;
}