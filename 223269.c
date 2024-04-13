void prep_transhuge_page(struct page *page)
{
	/*
	 * we use page->mapping and page->indexlru in second tail page
	 * as list_head: assuming THP order >= 2
	 */

	INIT_LIST_HEAD(page_deferred_list(page));
	set_compound_page_dtor(page, TRANSHUGE_PAGE_DTOR);
}