int isolate_lru_page(struct page *page, struct list_head *pagelist)
{
	int ret = -EBUSY;

	if (PageLRU(page)) {
		struct zone *zone = page_zone(page);

		spin_lock_irq(&zone->lru_lock);
		if (PageLRU(page) && get_page_unless_zero(page)) {
			ret = 0;
			ClearPageLRU(page);
			if (PageActive(page))
				del_page_from_active_list(zone, page);
			else
				del_page_from_inactive_list(zone, page);
			list_add_tail(&page->lru, pagelist);
		}
		spin_unlock_irq(&zone->lru_lock);
	}
	return ret;
}