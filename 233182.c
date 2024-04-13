put_node(
    FILE	*fd,		/* NULL when only counting */
    wordnode_T	*node,
    int		idx,
    int		regionmask,
    int		prefixtree)	/* TRUE for PREFIXTREE */
{
    int		newindex = idx;
    int		siblingcount = 0;
    wordnode_T	*np;
    int		flags;

    /* If "node" is zero the tree is empty. */
    if (node == NULL)
	return 0;

    /* Store the index where this node is written. */
    node->wn_u1.index = idx;

    /* Count the number of siblings. */
    for (np = node; np != NULL; np = np->wn_sibling)
	++siblingcount;

    /* Write the sibling count. */
    if (fd != NULL)
	putc(siblingcount, fd);				/* <siblingcount> */

    /* Write each sibling byte and optionally extra info. */
    for (np = node; np != NULL; np = np->wn_sibling)
    {
	if (np->wn_byte == 0)
	{
	    if (fd != NULL)
	    {
		/* For a NUL byte (end of word) write the flags etc. */
		if (prefixtree)
		{
		    /* In PREFIXTREE write the required affixID and the
		     * associated condition nr (stored in wn_region).  The
		     * byte value is misused to store the "rare" and "not
		     * combining" flags */
		    if (np->wn_flags == (short_u)PFX_FLAGS)
			putc(BY_NOFLAGS, fd);		/* <byte> */
		    else
		    {
			putc(BY_FLAGS, fd);		/* <byte> */
			putc(np->wn_flags, fd);		/* <pflags> */
		    }
		    putc(np->wn_affixID, fd);		/* <affixID> */
		    put_bytes(fd, (long_u)np->wn_region, 2); /* <prefcondnr> */
		}
		else
		{
		    /* For word trees we write the flag/region items. */
		    flags = np->wn_flags;
		    if (regionmask != 0 && np->wn_region != regionmask)
			flags |= WF_REGION;
		    if (np->wn_affixID != 0)
			flags |= WF_AFX;
		    if (flags == 0)
		    {
			/* word without flags or region */
			putc(BY_NOFLAGS, fd);			/* <byte> */
		    }
		    else
		    {
			if (np->wn_flags >= 0x100)
			{
			    putc(BY_FLAGS2, fd);		/* <byte> */
			    putc(flags, fd);			/* <flags> */
			    putc((unsigned)flags >> 8, fd);	/* <flags2> */
			}
			else
			{
			    putc(BY_FLAGS, fd);			/* <byte> */
			    putc(flags, fd);			/* <flags> */
			}
			if (flags & WF_REGION)
			    putc(np->wn_region, fd);		/* <region> */
			if (flags & WF_AFX)
			    putc(np->wn_affixID, fd);		/* <affixID> */
		    }
		}
	    }
	}
	else
	{
	    if (np->wn_child->wn_u1.index != 0
					 && np->wn_child->wn_u2.wnode != node)
	    {
		/* The child is written elsewhere, write the reference. */
		if (fd != NULL)
		{
		    putc(BY_INDEX, fd);			/* <byte> */
							/* <nodeidx> */
		    put_bytes(fd, (long_u)np->wn_child->wn_u1.index, 3);
		}
	    }
	    else if (np->wn_child->wn_u2.wnode == NULL)
		/* We will write the child below and give it an index. */
		np->wn_child->wn_u2.wnode = node;

	    if (fd != NULL)
		if (putc(np->wn_byte, fd) == EOF) /* <byte> or <xbyte> */
		{
		    EMSG(_(e_write));
		    return 0;
		}
	}
    }

    /* Space used in the array when reading: one for each sibling and one for
     * the count. */
    newindex += siblingcount + 1;

    /* Recursively dump the children of each sibling. */
    for (np = node; np != NULL; np = np->wn_sibling)
	if (np->wn_byte != 0 && np->wn_child->wn_u2.wnode == node)
	    newindex = put_node(fd, np->wn_child, newindex, regionmask,
								  prefixtree);

    return newindex;
}