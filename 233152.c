read_prefcond_section(FILE *fd, slang_T *lp)
{
    int		cnt;
    int		i;
    int		n;
    char_u	*p;
    char_u	buf[MAXWLEN + 1];

    /* <prefcondcnt> <prefcond> ... */
    cnt = get2c(fd);					/* <prefcondcnt> */
    if (cnt <= 0)
	return SP_FORMERROR;

    lp->sl_prefprog = (regprog_T **)alloc_clear(
					 (unsigned)sizeof(regprog_T *) * cnt);
    if (lp->sl_prefprog == NULL)
	return SP_OTHERERROR;
    lp->sl_prefixcnt = cnt;

    for (i = 0; i < cnt; ++i)
    {
	/* <prefcond> : <condlen> <condstr> */
	n = getc(fd);					/* <condlen> */
	if (n < 0 || n >= MAXWLEN)
	    return SP_FORMERROR;

	/* When <condlen> is zero we have an empty condition.  Otherwise
	 * compile the regexp program used to check for the condition. */
	if (n > 0)
	{
	    buf[0] = '^';	    /* always match at one position only */
	    p = buf + 1;
	    while (n-- > 0)
		*p++ = getc(fd);			/* <condstr> */
	    *p = NUL;
	    lp->sl_prefprog[i] = vim_regcomp(buf, RE_MAGIC + RE_STRING);
	}
    }
    return 0;
}