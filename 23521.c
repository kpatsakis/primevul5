static inline void move_to_lru(struct page *page)
{
	if (PageActive(page)) {
		/*
		 * lru_cache_add_active checks that
		 * the PG_active bit is off.
		 */
		ClearPageActive(page);
		lru_cache_add_active(page);
	} else {
		lru_cache_add(page);
	}
	put_page(page);
}