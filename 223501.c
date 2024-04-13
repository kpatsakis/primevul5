static int migrate_page_add(struct page *page, struct list_head *pagelist,
				unsigned long flags)
{
	struct page *head = compound_head(page);
	/*
	 * Avoid migrating a page that is shared with others.
	 */
	if ((flags & MPOL_MF_MOVE_ALL) || page_mapcount(head) == 1) {
		if (!isolate_lru_page(head)) {
			list_add_tail(&head->lru, pagelist);
			mod_node_page_state(page_pgdat(head),
				NR_ISOLATED_ANON + page_is_file_cache(head),
				hpage_nr_pages(head));
		} else if (flags & MPOL_MF_STRICT) {
			/*
			 * Non-movable page may reach here.  And, there may be
			 * temporary off LRU pages or non-LRU movable pages.
			 * Treat them as unmovable pages since they can't be
			 * isolated, so they can't be moved at the moment.  It
			 * should return -EIO for this case too.
			 */
			return -EIO;
		}
	}

	return 0;
}