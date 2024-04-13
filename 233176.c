set_sofo(slang_T *lp, char_u *from, char_u *to)
{
    int		i;

#ifdef FEAT_MBYTE
    garray_T	*gap;
    char_u	*s;
    char_u	*p;
    int		c;
    int		*inp;

    if (has_mbyte)
    {
	/* Use "sl_sal" as an array with 256 pointers to a list of wide
	 * characters.  The index is the low byte of the character.
	 * The list contains from-to pairs with a terminating NUL.
	 * sl_sal_first[] is used for latin1 "from" characters. */
	gap = &lp->sl_sal;
	ga_init2(gap, sizeof(int *), 1);
	if (ga_grow(gap, 256) == FAIL)
	    return SP_OTHERERROR;
	vim_memset(gap->ga_data, 0, sizeof(int *) * 256);
	gap->ga_len = 256;

	/* First count the number of items for each list.  Temporarily use
	 * sl_sal_first[] for this. */
	for (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    mb_cptr_adv(s);
	    if (c >= 256)
		++lp->sl_sal_first[c & 0xff];
	}
	if (*p != NUL || *s != NUL)	    /* lengths differ */
	    return SP_FORMERROR;

	/* Allocate the lists. */
	for (i = 0; i < 256; ++i)
	    if (lp->sl_sal_first[i] > 0)
	    {
		p = alloc(sizeof(int) * (lp->sl_sal_first[i] * 2 + 1));
		if (p == NULL)
		    return SP_OTHERERROR;
		((int **)gap->ga_data)[i] = (int *)p;
		*(int *)p = 0;
	    }

	/* Put the characters up to 255 in sl_sal_first[] the rest in a sl_sal
	 * list. */
	vim_memset(lp->sl_sal_first, 0, sizeof(salfirst_T) * 256);
	for (p = from, s = to; *p != NUL && *s != NUL; )
	{
	    c = mb_cptr2char_adv(&p);
	    i = mb_cptr2char_adv(&s);
	    if (c >= 256)
	    {
		/* Append the from-to chars at the end of the list with
		 * the low byte. */
		inp = ((int **)gap->ga_data)[c & 0xff];
		while (*inp != 0)
		    ++inp;
		*inp++ = c;		/* from char */
		*inp++ = i;		/* to char */
		*inp++ = NUL;		/* NUL at the end */
	    }
	    else
		/* mapping byte to char is done in sl_sal_first[] */
		lp->sl_sal_first[c] = i;
	}
    }
    else
#endif
    {
	/* mapping bytes to bytes is done in sl_sal_first[] */
	if (STRLEN(from) != STRLEN(to))
	    return SP_FORMERROR;

	for (i = 0; to[i] != NUL; ++i)
	    lp->sl_sal_first[from[i]] = to[i];
	lp->sl_sal.ga_len = 1;		/* indicates we have soundfolding */
    }

    return 0;
}