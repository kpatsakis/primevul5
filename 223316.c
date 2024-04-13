static unsigned long deferred_split_scan(struct shrinker *shrink,
		struct shrink_control *sc)
{
	struct pglist_data *pgdata = NODE_DATA(sc->nid);
	struct deferred_split *ds_queue = &pgdata->deferred_split_queue;
	unsigned long flags;
	LIST_HEAD(list), *pos, *next;
	struct page *page;
	int split = 0;

#ifdef CONFIG_MEMCG
	if (sc->memcg)
		ds_queue = &sc->memcg->deferred_split_queue;
#endif

	spin_lock_irqsave(&ds_queue->split_queue_lock, flags);
	/* Take pin on all head pages to avoid freeing them under us */
	list_for_each_safe(pos, next, &ds_queue->split_queue) {
		page = list_entry((void *)pos, struct page, mapping);
		page = compound_head(page);
		if (get_page_unless_zero(page)) {
			list_move(page_deferred_list(page), &list);
		} else {
			/* We lost race with put_compound_page() */
			list_del_init(page_deferred_list(page));
			ds_queue->split_queue_len--;
		}
		if (!--sc->nr_to_scan)
			break;
	}
	spin_unlock_irqrestore(&ds_queue->split_queue_lock, flags);

	list_for_each_safe(pos, next, &list) {
		page = list_entry((void *)pos, struct page, mapping);
		if (!trylock_page(page))
			goto next;
		/* split_huge_page() removes page from list on success */
		if (!split_huge_page(page))
			split++;
		unlock_page(page);
next:
		put_page(page);
	}

	spin_lock_irqsave(&ds_queue->split_queue_lock, flags);
	list_splice_tail(&list, &ds_queue->split_queue);
	spin_unlock_irqrestore(&ds_queue->split_queue_lock, flags);

	/*
	 * Stop shrinker if we didn't split any page, but the queue is empty.
	 * This can happen if pages were freed under us.
	 */
	if (!split && list_empty(&ds_queue->split_queue))
		return SHRINK_STOP;
	return split;
}