static void unmap_page(struct page *page)
{
	enum ttu_flags ttu_flags = TTU_IGNORE_MLOCK | TTU_IGNORE_ACCESS |
		TTU_RMAP_LOCKED | TTU_SPLIT_HUGE_PMD;
	bool unmap_success;

	VM_BUG_ON_PAGE(!PageHead(page), page);

	if (PageAnon(page))
		ttu_flags |= TTU_SPLIT_FREEZE;

	unmap_success = try_to_unmap(page, ttu_flags);
	VM_BUG_ON_PAGE(!unmap_success, page);
}