static long kernel_mbind(unsigned long start, unsigned long len,
			 unsigned long mode, const unsigned long __user *nmask,
			 unsigned long maxnode, unsigned int flags)
{
	nodemask_t nodes;
	int err;
	unsigned short mode_flags;

	start = untagged_addr(start);
	mode_flags = mode & MPOL_MODE_FLAGS;
	mode &= ~MPOL_MODE_FLAGS;
	if (mode >= MPOL_MAX)
		return -EINVAL;
	if ((mode_flags & MPOL_F_STATIC_NODES) &&
	    (mode_flags & MPOL_F_RELATIVE_NODES))
		return -EINVAL;
	err = get_nodes(&nodes, nmask, maxnode);
	if (err)
		return err;
	return do_mbind(start, len, mode, mode_flags, &nodes, flags);
}