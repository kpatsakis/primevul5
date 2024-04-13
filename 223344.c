void free_transhuge_page(struct page *page)
{
	struct deferred_split *ds_queue = get_deferred_split_queue(page);
	unsigned long flags;

	spin_lock_irqsave(&ds_queue->split_queue_lock, flags);
	if (!list_empty(page_deferred_list(page))) {
		ds_queue->split_queue_len--;
		list_del(page_deferred_list(page));
	}
	spin_unlock_irqrestore(&ds_queue->split_queue_lock, flags);
	free_compound_page(page);
}