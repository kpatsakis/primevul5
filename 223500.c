int huge_node(struct vm_area_struct *vma, unsigned long addr, gfp_t gfp_flags,
				struct mempolicy **mpol, nodemask_t **nodemask)
{
	int nid;

	*mpol = get_vma_policy(vma, addr);
	*nodemask = NULL;	/* assume !MPOL_BIND */

	if (unlikely((*mpol)->mode == MPOL_INTERLEAVE)) {
		nid = interleave_nid(*mpol, vma, addr,
					huge_page_shift(hstate_vma(vma)));
	} else {
		nid = policy_node(gfp_flags, *mpol, numa_node_id());
		if ((*mpol)->mode == MPOL_BIND)
			*nodemask = &(*mpol)->v.nodes;
	}
	return nid;
}