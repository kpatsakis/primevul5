bool init_nodemask_of_mempolicy(nodemask_t *mask)
{
	struct mempolicy *mempolicy;
	int nid;

	if (!(mask && current->mempolicy))
		return false;

	task_lock(current);
	mempolicy = current->mempolicy;
	switch (mempolicy->mode) {
	case MPOL_PREFERRED:
		if (mempolicy->flags & MPOL_F_LOCAL)
			nid = numa_node_id();
		else
			nid = mempolicy->v.preferred_node;
		init_nodemask_of_node(mask, nid);
		break;

	case MPOL_BIND:
		/* Fall through */
	case MPOL_INTERLEAVE:
		*mask =  mempolicy->v.nodes;
		break;

	default:
		BUG();
	}
	task_unlock(current);

	return true;
}