set_sal_first(slang_T *lp)
{
    salfirst_T	*sfirst;
    int		i;
    salitem_T	*smp;
    int		c;
    garray_T	*gap = &lp->sl_sal;

    sfirst = lp->sl_sal_first;
    for (i = 0; i < 256; ++i)
	sfirst[i] = -1;
    smp = (salitem_T *)gap->ga_data;
    for (i = 0; i < gap->ga_len; ++i)
    {
#ifdef FEAT_MBYTE
	if (has_mbyte)
	    /* Use the lowest byte of the first character.  For latin1 it's
	     * the character, for other encodings it should differ for most
	     * characters. */
	    c = *smp[i].sm_lead_w & 0xff;
	else
#endif
	    c = *smp[i].sm_lead;
	if (sfirst[c] == -1)
	{
	    sfirst[c] = i;
#ifdef FEAT_MBYTE
	    if (has_mbyte)
	    {
		int		n;

		/* Make sure all entries with this byte are following each
		 * other.  Move the ones that are in the wrong position.  Do
		 * keep the same ordering! */
		while (i + 1 < gap->ga_len
				       && (*smp[i + 1].sm_lead_w & 0xff) == c)
		    /* Skip over entry with same index byte. */
		    ++i;

		for (n = 1; i + n < gap->ga_len; ++n)
		    if ((*smp[i + n].sm_lead_w & 0xff) == c)
		    {
			salitem_T  tsal;

			/* Move entry with same index byte after the entries
			 * we already found. */
			++i;
			--n;
			tsal = smp[i + n];
			mch_memmove(smp + i + 1, smp + i,
						       sizeof(salitem_T) * n);
			smp[i] = tsal;
		    }
	    }
#endif
	}
    }
}