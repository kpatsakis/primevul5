int putback_lru_pages(struct list_head *l)
{
	struct page *page;
	struct page *page2;
	int count = 0;

	list_for_each_entry_safe(page, page2, l, lru) {
		list_del(&page->lru);
		move_to_lru(page);
		count++;
	}
	return count;
}