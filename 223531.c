static struct page *alloc_page_interleave(gfp_t gfp, unsigned order,
					unsigned nid)
{
	struct page *page;

	page = __alloc_pages(gfp, order, nid);
	/* skip NUMA_INTERLEAVE_HIT counter update if numa stats is disabled */
	if (!static_branch_likely(&vm_numa_stat_key))
		return page;
	if (page && page_to_nid(page) == nid) {
		preempt_disable();
		__inc_numa_state(page_zone(page), NUMA_INTERLEAVE_HIT);
		preempt_enable();
	}
	return page;
}