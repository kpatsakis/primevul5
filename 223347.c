static inline struct deferred_split *get_deferred_split_queue(struct page *page)
{
	struct mem_cgroup *memcg = compound_head(page)->mem_cgroup;
	struct pglist_data *pgdat = NODE_DATA(page_to_nid(page));

	if (memcg)
		return &memcg->deferred_split_queue;
	else
		return &pgdat->deferred_split_queue;
}