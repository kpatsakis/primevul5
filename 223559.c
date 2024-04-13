static inline bool queue_pages_required(struct page *page,
					struct queue_pages *qp)
{
	int nid = page_to_nid(page);
	unsigned long flags = qp->flags;

	return node_isset(nid, *qp->nmask) == !(flags & MPOL_MF_INVERT);
}