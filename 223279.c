static struct page *get_huge_zero_page(void)
{
	struct page *zero_page;
retry:
	if (likely(atomic_inc_not_zero(&huge_zero_refcount)))
		return READ_ONCE(huge_zero_page);

	zero_page = alloc_pages((GFP_TRANSHUGE | __GFP_ZERO) & ~__GFP_MOVABLE,
			HPAGE_PMD_ORDER);
	if (!zero_page) {
		count_vm_event(THP_ZERO_PAGE_ALLOC_FAILED);
		return NULL;
	}
	count_vm_event(THP_ZERO_PAGE_ALLOC);
	preempt_disable();
	if (cmpxchg(&huge_zero_page, NULL, zero_page)) {
		preempt_enable();
		__free_pages(zero_page, compound_order(zero_page));
		goto retry;
	}

	/* We take additional reference here. It will be put back by shrinker */
	atomic_set(&huge_zero_refcount, 2);
	preempt_enable();
	return READ_ONCE(huge_zero_page);
}