static int get_nodes(nodemask_t *nodes, const unsigned long __user *nmask,
		     unsigned long maxnode)
{
	unsigned long k;
	unsigned long t;
	unsigned long nlongs;
	unsigned long endmask;

	--maxnode;
	nodes_clear(*nodes);
	if (maxnode == 0 || !nmask)
		return 0;
	if (maxnode > PAGE_SIZE*BITS_PER_BYTE)
		return -EINVAL;

	nlongs = BITS_TO_LONGS(maxnode);
	if ((maxnode % BITS_PER_LONG) == 0)
		endmask = ~0UL;
	else
		endmask = (1UL << (maxnode % BITS_PER_LONG)) - 1;

	/*
	 * When the user specified more nodes than supported just check
	 * if the non supported part is all zero.
	 *
	 * If maxnode have more longs than MAX_NUMNODES, check
	 * the bits in that area first. And then go through to
	 * check the rest bits which equal or bigger than MAX_NUMNODES.
	 * Otherwise, just check bits [MAX_NUMNODES, maxnode).
	 */
	if (nlongs > BITS_TO_LONGS(MAX_NUMNODES)) {
		for (k = BITS_TO_LONGS(MAX_NUMNODES); k < nlongs; k++) {
			if (get_user(t, nmask + k))
				return -EFAULT;
			if (k == nlongs - 1) {
				if (t & endmask)
					return -EINVAL;
			} else if (t)
				return -EINVAL;
		}
		nlongs = BITS_TO_LONGS(MAX_NUMNODES);
		endmask = ~0UL;
	}

	if (maxnode > MAX_NUMNODES && MAX_NUMNODES % BITS_PER_LONG != 0) {
		unsigned long valid_mask = endmask;

		valid_mask &= ~((1UL << (MAX_NUMNODES % BITS_PER_LONG)) - 1);
		if (get_user(t, nmask + nlongs - 1))
			return -EFAULT;
		if (t & valid_mask)
			return -EINVAL;
	}

	if (copy_from_user(nodes_addr(*nodes), nmask, nlongs*sizeof(unsigned long)))
		return -EFAULT;
	nodes_addr(*nodes)[nlongs-1] &= endmask;
	return 0;
}