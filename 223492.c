static int policy_node(gfp_t gfp, struct mempolicy *policy,
								int nd)
{
	if (policy->mode == MPOL_PREFERRED && !(policy->flags & MPOL_F_LOCAL))
		nd = policy->v.preferred_node;
	else {
		/*
		 * __GFP_THISNODE shouldn't even be used with the bind policy
		 * because we might easily break the expectation to stay on the
		 * requested node and not break the policy.
		 */
		WARN_ON_ONCE(policy->mode == MPOL_BIND && (gfp & __GFP_THISNODE));
	}

	return nd;
}