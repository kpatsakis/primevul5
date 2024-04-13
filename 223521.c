alloc_pages_vma(gfp_t gfp, int order, struct vm_area_struct *vma,
		unsigned long addr, int node, bool hugepage)
{
	struct mempolicy *pol;
	struct page *page;
	int preferred_nid;
	nodemask_t *nmask;

	pol = get_vma_policy(vma, addr);

	if (pol->mode == MPOL_INTERLEAVE) {
		unsigned nid;

		nid = interleave_nid(pol, vma, addr, PAGE_SHIFT + order);
		mpol_cond_put(pol);
		page = alloc_page_interleave(gfp, order, nid);
		goto out;
	}

	if (unlikely(IS_ENABLED(CONFIG_TRANSPARENT_HUGEPAGE) && hugepage)) {
		int hpage_node = node;

		/*
		 * For hugepage allocation and non-interleave policy which
		 * allows the current node (or other explicitly preferred
		 * node) we only try to allocate from the current/preferred
		 * node and don't fall back to other nodes, as the cost of
		 * remote accesses would likely offset THP benefits.
		 *
		 * If the policy is interleave, or does not allow the current
		 * node in its nodemask, we allocate the standard way.
		 */
		if (pol->mode == MPOL_PREFERRED && !(pol->flags & MPOL_F_LOCAL))
			hpage_node = pol->v.preferred_node;

		nmask = policy_nodemask(gfp, pol);
		if (!nmask || node_isset(hpage_node, *nmask)) {
			mpol_cond_put(pol);
			/*
			 * First, try to allocate THP only on local node, but
			 * don't reclaim unnecessarily, just compact.
			 */
			page = __alloc_pages_node(hpage_node,
				gfp | __GFP_THISNODE | __GFP_NORETRY, order);

			/*
			 * If hugepage allocations are configured to always
			 * synchronous compact or the vma has been madvised
			 * to prefer hugepage backing, retry allowing remote
			 * memory with both reclaim and compact as well.
			 */
			if (!page && (gfp & __GFP_DIRECT_RECLAIM))
				page = __alloc_pages_node(hpage_node,
								gfp, order);

			goto out;
		}
	}

	nmask = policy_nodemask(gfp, pol);
	preferred_nid = policy_node(gfp, pol, node);
	page = __alloc_pages_nodemask(gfp, order, preferred_nid, nmask);
	mpol_cond_put(pol);
out:
	return page;
}