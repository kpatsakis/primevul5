static long kernel_set_mempolicy(int mode, const unsigned long __user *nmask,
				 unsigned long maxnode)
{
	int err;
	nodemask_t nodes;
	unsigned short flags;

	flags = mode & MPOL_MODE_FLAGS;
	mode &= ~MPOL_MODE_FLAGS;
	if ((unsigned int)mode >= MPOL_MAX)
		return -EINVAL;
	if ((flags & MPOL_F_STATIC_NODES) && (flags & MPOL_F_RELATIVE_NODES))
		return -EINVAL;
	err = get_nodes(&nodes, nmask, maxnode);
	if (err)
		return err;
	return do_set_mempolicy(mode, flags, &nodes);
}