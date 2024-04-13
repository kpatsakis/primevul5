read_tree_node(
    FILE	*fd,
    char_u	*byts,
    idx_T	*idxs,
    int		maxidx,		    /* size of arrays */
    idx_T	startidx,	    /* current index in "byts" and "idxs" */
    int		prefixtree,	    /* TRUE for reading PREFIXTREE */
    int		maxprefcondnr)	    /* maximum for <prefcondnr> */
{
    int		len;
    int		i;
    int		n;
    idx_T	idx = startidx;
    int		c;
    int		c2;
#define SHARED_MASK	0x8000000

    len = getc(fd);					/* <siblingcount> */
    if (len <= 0)
	return SP_TRUNCERROR;

    if (startidx + len >= maxidx)
	return SP_FORMERROR;
    byts[idx++] = len;

    /* Read the byte values, flag/region bytes and shared indexes. */
    for (i = 1; i <= len; ++i)
    {
	c = getc(fd);					/* <byte> */
	if (c < 0)
	    return SP_TRUNCERROR;
	if (c <= BY_SPECIAL)
	{
	    if (c == BY_NOFLAGS && !prefixtree)
	    {
		/* No flags, all regions. */
		idxs[idx] = 0;
		c = 0;
	    }
	    else if (c != BY_INDEX)
	    {
		if (prefixtree)
		{
		    /* Read the optional pflags byte, the prefix ID and the
		     * condition nr.  In idxs[] store the prefix ID in the low
		     * byte, the condition index shifted up 8 bits, the flags
		     * shifted up 24 bits. */
		    if (c == BY_FLAGS)
			c = getc(fd) << 24;		/* <pflags> */
		    else
			c = 0;

		    c |= getc(fd);			/* <affixID> */

		    n = get2c(fd);			/* <prefcondnr> */
		    if (n >= maxprefcondnr)
			return SP_FORMERROR;
		    c |= (n << 8);
		}
		else /* c must be BY_FLAGS or BY_FLAGS2 */
		{
		    /* Read flags and optional region and prefix ID.  In
		     * idxs[] the flags go in the low two bytes, region above
		     * that and prefix ID above the region. */
		    c2 = c;
		    c = getc(fd);			/* <flags> */
		    if (c2 == BY_FLAGS2)
			c = (getc(fd) << 8) + c;	/* <flags2> */
		    if (c & WF_REGION)
			c = (getc(fd) << 16) + c;	/* <region> */
		    if (c & WF_AFX)
			c = (getc(fd) << 24) + c;	/* <affixID> */
		}

		idxs[idx] = c;
		c = 0;
	    }
	    else /* c == BY_INDEX */
	    {
							/* <nodeidx> */
		n = get3c(fd);
		if (n < 0 || n >= maxidx)
		    return SP_FORMERROR;
		idxs[idx] = n + SHARED_MASK;
		c = getc(fd);				/* <xbyte> */
	    }
	}
	byts[idx++] = c;
    }

    /* Recursively read the children for non-shared siblings.
     * Skip the end-of-word ones (zero byte value) and the shared ones (and
     * remove SHARED_MASK) */
    for (i = 1; i <= len; ++i)
	if (byts[startidx + i] != 0)
	{
	    if (idxs[startidx + i] & SHARED_MASK)
		idxs[startidx + i] &= ~SHARED_MASK;
	    else
	    {
		idxs[startidx + i] = idx;
		idx = read_tree_node(fd, byts, idxs, maxidx, idx,
						     prefixtree, maxprefcondnr);
		if (idx < 0)
		    break;
	    }
	}

    return idx;
}