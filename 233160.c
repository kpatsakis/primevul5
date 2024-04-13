sug_filltable(
    spellinfo_T	*spin,
    wordnode_T	*node,
    int		startwordnr,
    garray_T	*gap)	    /* place to store line of numbers */
{
    wordnode_T	*p, *np;
    int		wordnr = startwordnr;
    int		nr;
    int		prev_nr;

    for (p = node; p != NULL; p = p->wn_sibling)
    {
	if (p->wn_byte == NUL)
	{
	    gap->ga_len = 0;
	    prev_nr = 0;
	    for (np = p; np != NULL && np->wn_byte == NUL; np = np->wn_sibling)
	    {
		if (ga_grow(gap, 10) == FAIL)
		    return -1;

		nr = (np->wn_flags << 16) + (np->wn_region & 0xffff);
		/* Compute the offset from the previous nr and store the
		 * offset in a way that it takes a minimum number of bytes.
		 * It's a bit like utf-8, but without the need to mark
		 * following bytes. */
		nr -= prev_nr;
		prev_nr += nr;
		gap->ga_len += offset2bytes(nr,
					 (char_u *)gap->ga_data + gap->ga_len);
	    }

	    /* add the NUL byte */
	    ((char_u *)gap->ga_data)[gap->ga_len++] = NUL;

	    if (ml_append_buf(spin->si_spellbuf, (linenr_T)wordnr,
				     gap->ga_data, gap->ga_len, TRUE) == FAIL)
		return -1;
	    ++wordnr;

	    /* Remove extra NUL entries, we no longer need them. We don't
	     * bother freeing the nodes, the won't be reused anyway. */
	    while (p->wn_sibling != NULL && p->wn_sibling->wn_byte == NUL)
		p->wn_sibling = p->wn_sibling->wn_sibling;

	    /* Clear the flags on the remaining NUL node, so that compression
	     * works a lot better. */
	    p->wn_flags = 0;
	    p->wn_region = 0;
	}
	else
	{
	    wordnr = sug_filltable(spin, p->wn_child, wordnr, gap);
	    if (wordnr == -1)
		return -1;
	}
    }
    return wordnr;
}