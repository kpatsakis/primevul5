int mpol_misplaced(struct page *page, struct vm_area_struct *vma, unsigned long addr)
{
	struct mempolicy *pol;
	struct zoneref *z;
	int curnid = page_to_nid(page);
	unsigned long pgoff;
	int thiscpu = raw_smp_processor_id();
	int thisnid = cpu_to_node(thiscpu);
	int polnid = NUMA_NO_NODE;
	int ret = -1;

	pol = get_vma_policy(vma, addr);
	if (!(pol->flags & MPOL_F_MOF))
		goto out;

	switch (pol->mode) {
	case MPOL_INTERLEAVE:
		pgoff = vma->vm_pgoff;
		pgoff += (addr - vma->vm_start) >> PAGE_SHIFT;
		polnid = offset_il_node(pol, pgoff);
		break;

	case MPOL_PREFERRED:
		if (pol->flags & MPOL_F_LOCAL)
			polnid = numa_node_id();
		else
			polnid = pol->v.preferred_node;
		break;

	case MPOL_BIND:

		/*
		 * allows binding to multiple nodes.
		 * use current page if in policy nodemask,
		 * else select nearest allowed node, if any.
		 * If no allowed nodes, use current [!misplaced].
		 */
		if (node_isset(curnid, pol->v.nodes))
			goto out;
		z = first_zones_zonelist(
				node_zonelist(numa_node_id(), GFP_HIGHUSER),
				gfp_zone(GFP_HIGHUSER),
				&pol->v.nodes);
		polnid = zone_to_nid(z->zone);
		break;

	default:
		BUG();
	}

	/* Migrate the page towards the node whose CPU is referencing it */
	if (pol->flags & MPOL_F_MORON) {
		polnid = thisnid;

		if (!should_numa_migrate_memory(current, page, curnid, thiscpu))
			goto out;
	}

	if (curnid != polnid)
		ret = polnid;
out:
	mpol_cond_put(pol);

	return ret;
}