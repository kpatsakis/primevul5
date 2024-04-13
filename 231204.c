static bool vhost_net_page_frag_refill(struct vhost_net *net, unsigned int sz,
				       struct page_frag *pfrag, gfp_t gfp)
{
	if (pfrag->page) {
		if (pfrag->offset + sz <= pfrag->size)
			return true;
		__page_frag_cache_drain(pfrag->page, net->refcnt_bias);
	}

	pfrag->offset = 0;
	net->refcnt_bias = 0;
	if (SKB_FRAG_PAGE_ORDER) {
		/* Avoid direct reclaim but allow kswapd to wake */
		pfrag->page = alloc_pages((gfp & ~__GFP_DIRECT_RECLAIM) |
					  __GFP_COMP | __GFP_NOWARN |
					  __GFP_NORETRY,
					  SKB_FRAG_PAGE_ORDER);
		if (likely(pfrag->page)) {
			pfrag->size = PAGE_SIZE << SKB_FRAG_PAGE_ORDER;
			goto done;
		}
	}
	pfrag->page = alloc_page(gfp);
	if (likely(pfrag->page)) {
		pfrag->size = PAGE_SIZE;
		goto done;
	}
	return false;

done:
	net->refcnt_bias = USHRT_MAX;
	page_ref_add(pfrag->page, USHRT_MAX - 1);
	return true;
}