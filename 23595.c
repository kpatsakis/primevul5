static int migrate_page_move_mapping(struct address_space *mapping,
		struct page *newpage, struct page *page)
{
	void **pslot;

	if (!mapping) {
		/* Anonymous page without mapping */
		if (page_count(page) != 1)
			return -EAGAIN;
		return 0;
	}

	write_lock_irq(&mapping->tree_lock);

	pslot = radix_tree_lookup_slot(&mapping->page_tree,
 					page_index(page));

	if (page_count(page) != 2 + !!PagePrivate(page) ||
			(struct page *)radix_tree_deref_slot(pslot) != page) {
		write_unlock_irq(&mapping->tree_lock);
		return -EAGAIN;
	}

	/*
	 * Now we know that no one else is looking at the page.
	 */
	get_page(newpage);	/* add cache reference */
#ifdef CONFIG_SWAP
	if (PageSwapCache(page)) {
		SetPageSwapCache(newpage);
		set_page_private(newpage, page_private(page));
	}
#endif

	radix_tree_replace_slot(pslot, newpage);

	/*
	 * Drop cache reference from old page.
	 * We know this isn't the last reference.
	 */
	__put_page(page);

	/*
	 * If moved to a different zone then also account
	 * the page for that zone. Other VM counters will be
	 * taken care of when we establish references to the
	 * new page and drop references to the old page.
	 *
	 * Note that anonymous pages are accounted for
	 * via NR_FILE_PAGES and NR_ANON_PAGES if they
	 * are mapped to swap space.
	 */
	__dec_zone_page_state(page, NR_FILE_PAGES);
	__inc_zone_page_state(newpage, NR_FILE_PAGES);

	write_unlock_irq(&mapping->tree_lock);

	return 0;
}