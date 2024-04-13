void deferred_split_huge_page(struct page *page)
{
	struct deferred_split *ds_queue = get_deferred_split_queue(page);
#ifdef CONFIG_MEMCG
	struct mem_cgroup *memcg = compound_head(page)->mem_cgroup;
#endif
	unsigned long flags;

	VM_BUG_ON_PAGE(!PageTransHuge(page), page);

	/*
	 * The try_to_unmap() in page reclaim path might reach here too,
	 * this may cause a race condition to corrupt deferred split queue.
	 * And, if page reclaim is already handling the same page, it is
	 * unnecessary to handle it again in shrinker.
	 *
	 * Check PageSwapCache to determine if the page is being
	 * handled by page reclaim since THP swap would add the page into
	 * swap cache before calling try_to_unmap().
	 */
	if (PageSwapCache(page))
		return;

	spin_lock_irqsave(&ds_queue->split_queue_lock, flags);
	if (list_empty(page_deferred_list(page))) {
		count_vm_event(THP_DEFERRED_SPLIT_PAGE);
		list_add_tail(page_deferred_list(page), &ds_queue->split_queue);
		ds_queue->split_queue_len++;
#ifdef CONFIG_MEMCG
		if (memcg)
			memcg_set_shrinker_bit(memcg, page_to_nid(page),
					       deferred_split_shrinker.id);
#endif
	}
	spin_unlock_irqrestore(&ds_queue->split_queue_lock, flags);
}