static void __split_huge_page_tail(struct page *head, int tail,
		struct lruvec *lruvec, struct list_head *list)
{
	struct page *page_tail = head + tail;

	VM_BUG_ON_PAGE(atomic_read(&page_tail->_mapcount) != -1, page_tail);

	/*
	 * Clone page flags before unfreezing refcount.
	 *
	 * After successful get_page_unless_zero() might follow flags change,
	 * for exmaple lock_page() which set PG_waiters.
	 */
	page_tail->flags &= ~PAGE_FLAGS_CHECK_AT_PREP;
	page_tail->flags |= (head->flags &
			((1L << PG_referenced) |
			 (1L << PG_swapbacked) |
			 (1L << PG_swapcache) |
			 (1L << PG_mlocked) |
			 (1L << PG_uptodate) |
			 (1L << PG_active) |
			 (1L << PG_workingset) |
			 (1L << PG_locked) |
			 (1L << PG_unevictable) |
			 (1L << PG_dirty)));

	/* ->mapping in first tail page is compound_mapcount */
	VM_BUG_ON_PAGE(tail > 2 && page_tail->mapping != TAIL_MAPPING,
			page_tail);
	page_tail->mapping = head->mapping;
	page_tail->index = head->index + tail;

	/* Page flags must be visible before we make the page non-compound. */
	smp_wmb();

	/*
	 * Clear PageTail before unfreezing page refcount.
	 *
	 * After successful get_page_unless_zero() might follow put_page()
	 * which needs correct compound_head().
	 */
	clear_compound_head(page_tail);

	/* Finally unfreeze refcount. Additional reference from page cache. */
	page_ref_unfreeze(page_tail, 1 + (!PageAnon(head) ||
					  PageSwapCache(head)));

	if (page_is_young(head))
		set_page_young(page_tail);
	if (page_is_idle(head))
		set_page_idle(page_tail);

	page_cpupid_xchg_last(page_tail, page_cpupid_last(head));

	/*
	 * always add to the tail because some iterators expect new
	 * pages to show after the currently processed elements - e.g.
	 * migrate_pages
	 */
	lru_add_page_tail(head, page_tail, lruvec, list);
}