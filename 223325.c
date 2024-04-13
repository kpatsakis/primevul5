static inline struct deferred_split *get_deferred_split_queue(struct page *page)
{
	struct pglist_data *pgdat = NODE_DATA(page_to_nid(page));

	return &pgdat->deferred_split_queue;
}