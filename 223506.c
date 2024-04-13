struct page *alloc_new_node_page(struct page *page, unsigned long node)
{
	if (PageHuge(page))
		return alloc_huge_page_node(page_hstate(compound_head(page)),
					node);
	else if (PageTransHuge(page)) {
		struct page *thp;

		thp = alloc_pages_node(node,
			(GFP_TRANSHUGE | __GFP_THISNODE),
			HPAGE_PMD_ORDER);
		if (!thp)
			return NULL;
		prep_transhuge_page(thp);
		return thp;
	} else
		return __alloc_pages_node(node, GFP_HIGHUSER_MOVABLE |
						    __GFP_THISNODE, 0);
}