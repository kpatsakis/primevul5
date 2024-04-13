read_compound(FILE *fd, slang_T *slang, int len)
{
    int		todo = len;
    int		c;
    int		atstart;
    char_u	*pat;
    char_u	*pp;
    char_u	*cp;
    char_u	*ap;
    char_u	*crp;
    int		cnt;
    garray_T	*gap;

    if (todo < 2)
	return SP_FORMERROR;	/* need at least two bytes */

    --todo;
    c = getc(fd);					/* <compmax> */
    if (c < 2)
	c = MAXWLEN;
    slang->sl_compmax = c;

    --todo;
    c = getc(fd);					/* <compminlen> */
    if (c < 1)
	c = 0;
    slang->sl_compminlen = c;

    --todo;
    c = getc(fd);					/* <compsylmax> */
    if (c < 1)
	c = MAXWLEN;
    slang->sl_compsylmax = c;

    c = getc(fd);					/* <compoptions> */
    if (c != 0)
	ungetc(c, fd);	    /* be backwards compatible with Vim 7.0b */
    else
    {
	--todo;
	c = getc(fd);	    /* only use the lower byte for now */
	--todo;
	slang->sl_compoptions = c;

	gap = &slang->sl_comppat;
	c = get2c(fd);					/* <comppatcount> */
	todo -= 2;
	ga_init2(gap, sizeof(char_u *), c);
	if (ga_grow(gap, c) == OK)
	    while (--c >= 0)
	    {
		((char_u **)(gap->ga_data))[gap->ga_len++] =
						 read_cnt_string(fd, 1, &cnt);
					    /* <comppatlen> <comppattext> */
		if (cnt < 0)
		    return cnt;
		todo -= cnt + 1;
	    }
    }
    if (todo < 0)
	return SP_FORMERROR;

    /* Turn the COMPOUNDRULE items into a regexp pattern:
     * "a[bc]/a*b+" -> "^\(a[bc]\|a*b\+\)$".
     * Inserting backslashes may double the length, "^\(\)$<Nul>" is 7 bytes.
     * Conversion to utf-8 may double the size. */
    c = todo * 2 + 7;
#ifdef FEAT_MBYTE
    if (enc_utf8)
	c += todo * 2;
#endif
    pat = alloc((unsigned)c);
    if (pat == NULL)
	return SP_OTHERERROR;

    /* We also need a list of all flags that can appear at the start and one
     * for all flags. */
    cp = alloc(todo + 1);
    if (cp == NULL)
    {
	vim_free(pat);
	return SP_OTHERERROR;
    }
    slang->sl_compstartflags = cp;
    *cp = NUL;

    ap = alloc(todo + 1);
    if (ap == NULL)
    {
	vim_free(pat);
	return SP_OTHERERROR;
    }
    slang->sl_compallflags = ap;
    *ap = NUL;

    /* And a list of all patterns in their original form, for checking whether
     * compounding may work in match_compoundrule().  This is freed when we
     * encounter a wildcard, the check doesn't work then. */
    crp = alloc(todo + 1);
    slang->sl_comprules = crp;

    pp = pat;
    *pp++ = '^';
    *pp++ = '\\';
    *pp++ = '(';

    atstart = 1;
    while (todo-- > 0)
    {
	c = getc(fd);					/* <compflags> */
	if (c == EOF)
	{
	    vim_free(pat);
	    return SP_TRUNCERROR;
	}

	/* Add all flags to "sl_compallflags". */
	if (vim_strchr((char_u *)"?*+[]/", c) == NULL
		&& !byte_in_str(slang->sl_compallflags, c))
	{
	    *ap++ = c;
	    *ap = NUL;
	}

	if (atstart != 0)
	{
	    /* At start of item: copy flags to "sl_compstartflags".  For a
	     * [abc] item set "atstart" to 2 and copy up to the ']'. */
	    if (c == '[')
		atstart = 2;
	    else if (c == ']')
		atstart = 0;
	    else
	    {
		if (!byte_in_str(slang->sl_compstartflags, c))
		{
		    *cp++ = c;
		    *cp = NUL;
		}
		if (atstart == 1)
		    atstart = 0;
	    }
	}

	/* Copy flag to "sl_comprules", unless we run into a wildcard. */
	if (crp != NULL)
	{
	    if (c == '?' || c == '+' || c == '*')
	    {
		vim_free(slang->sl_comprules);
		slang->sl_comprules = NULL;
		crp = NULL;
	    }
	    else
		*crp++ = c;
	}

	if (c == '/')	    /* slash separates two items */
	{
	    *pp++ = '\\';
	    *pp++ = '|';
	    atstart = 1;
	}
	else		    /* normal char, "[abc]" and '*' are copied as-is */
	{
	    if (c == '?' || c == '+' || c == '~')
		*pp++ = '\\';	    /* "a?" becomes "a\?", "a+" becomes "a\+" */
#ifdef FEAT_MBYTE
	    if (enc_utf8)
		pp += mb_char2bytes(c, pp);
	    else
#endif
		*pp++ = c;
	}
    }

    *pp++ = '\\';
    *pp++ = ')';
    *pp++ = '$';
    *pp = NUL;

    if (crp != NULL)
	*crp = NUL;

    slang->sl_compprog = vim_regcomp(pat, RE_MAGIC + RE_STRING + RE_STRICT);
    vim_free(pat);
    if (slang->sl_compprog == NULL)
	return SP_FORMERROR;

    return 0;
}