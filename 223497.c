static int kernel_get_mempolicy(int __user *policy,
				unsigned long __user *nmask,
				unsigned long maxnode,
				unsigned long addr,
				unsigned long flags)
{
	int err;
	int uninitialized_var(pval);
	nodemask_t nodes;

	addr = untagged_addr(addr);

	if (nmask != NULL && maxnode < nr_node_ids)
		return -EINVAL;

	err = do_get_mempolicy(&pval, &nodes, addr, flags);

	if (err)
		return err;

	if (policy && put_user(pval, policy))
		return -EFAULT;

	if (nmask)
		err = copy_nodes_to_user(nmask, maxnode, &nodes);

	return err;
}