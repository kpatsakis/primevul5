static void mpol_rebind_policy(struct mempolicy *pol, const nodemask_t *newmask)
{
	if (!pol)
		return;
	if (!mpol_store_user_nodemask(pol) && !(pol->flags & MPOL_F_LOCAL) &&
	    nodes_equal(pol->w.cpuset_mems_allowed, *newmask))
		return;

	mpol_ops[pol->mode].rebind(pol, newmask);
}