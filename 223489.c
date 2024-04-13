static void mpol_rebind_nodemask(struct mempolicy *pol, const nodemask_t *nodes)
{
	nodemask_t tmp;

	if (pol->flags & MPOL_F_STATIC_NODES)
		nodes_and(tmp, pol->w.user_nodemask, *nodes);
	else if (pol->flags & MPOL_F_RELATIVE_NODES)
		mpol_relative_nodemask(&tmp, &pol->w.user_nodemask, nodes);
	else {
		nodes_remap(tmp, pol->v.nodes,pol->w.cpuset_mems_allowed,
								*nodes);
		pol->w.cpuset_mems_allowed = *nodes;
	}

	if (nodes_empty(tmp))
		tmp = *nodes;

	pol->v.nodes = tmp;
}