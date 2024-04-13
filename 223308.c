bool is_transparent_hugepage(struct page *page)
{
	if (!PageCompound(page))
		return 0;

	page = compound_head(page);
	return is_huge_zero_page(page) ||
	       page[1].compound_dtor == TRANSHUGE_PAGE_DTOR;
}