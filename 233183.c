read_sal_section(FILE *fd, slang_T *slang)
{
    int		i;
    int		cnt;
    garray_T	*gap;
    salitem_T	*smp;
    int		ccnt;
    char_u	*p;
    int		c = NUL;

    slang->sl_sofo = FALSE;

    i = getc(fd);				/* <salflags> */
    if (i & SAL_F0LLOWUP)
	slang->sl_followup = TRUE;
    if (i & SAL_COLLAPSE)
	slang->sl_collapse = TRUE;
    if (i & SAL_REM_ACCENTS)
	slang->sl_rem_accents = TRUE;

    cnt = get2c(fd);				/* <salcount> */
    if (cnt < 0)
	return SP_TRUNCERROR;

    gap = &slang->sl_sal;
    ga_init2(gap, sizeof(salitem_T), 10);
    if (ga_grow(gap, cnt + 1) == FAIL)
	return SP_OTHERERROR;

    /* <sal> : <salfromlen> <salfrom> <saltolen> <salto> */
    for (; gap->ga_len < cnt; ++gap->ga_len)
    {
	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	ccnt = getc(fd);			/* <salfromlen> */
	if (ccnt < 0)
	    return SP_TRUNCERROR;
	if ((p = alloc(ccnt + 2)) == NULL)
	    return SP_OTHERERROR;
	smp->sm_lead = p;

	/* Read up to the first special char into sm_lead. */
	for (i = 0; i < ccnt; ++i)
	{
	    c = getc(fd);			/* <salfrom> */
	    if (vim_strchr((char_u *)"0123456789(-<^$", c) != NULL)
		break;
	    *p++ = c;
	}
	smp->sm_leadlen = (int)(p - smp->sm_lead);
	*p++ = NUL;

	/* Put (abc) chars in sm_oneof, if any. */
	if (c == '(')
	{
	    smp->sm_oneof = p;
	    for (++i; i < ccnt; ++i)
	    {
		c = getc(fd);			/* <salfrom> */
		if (c == ')')
		    break;
		*p++ = c;
	    }
	    *p++ = NUL;
	    if (++i < ccnt)
		c = getc(fd);
	}
	else
	    smp->sm_oneof = NULL;

	/* Any following chars go in sm_rules. */
	smp->sm_rules = p;
	if (i < ccnt)
	    /* store the char we got while checking for end of sm_lead */
	    *p++ = c;
	for (++i; i < ccnt; ++i)
	    *p++ = getc(fd);			/* <salfrom> */
	*p++ = NUL;

	/* <saltolen> <salto> */
	smp->sm_to = read_cnt_string(fd, 1, &ccnt);
	if (ccnt < 0)
	{
	    vim_free(smp->sm_lead);
	    return ccnt;
	}

#ifdef FEAT_MBYTE
	if (has_mbyte)
	{
	    /* convert the multi-byte strings to wide char strings */
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = mb_charlen(smp->sm_lead);
	    if (smp->sm_oneof == NULL)
		smp->sm_oneof_w = NULL;
	    else
		smp->sm_oneof_w = mb_str2wide(smp->sm_oneof);
	    if (smp->sm_to == NULL)
		smp->sm_to_w = NULL;
	    else
		smp->sm_to_w = mb_str2wide(smp->sm_to);
	    if (smp->sm_lead_w == NULL
		    || (smp->sm_oneof_w == NULL && smp->sm_oneof != NULL)
		    || (smp->sm_to_w == NULL && smp->sm_to != NULL))
	    {
		vim_free(smp->sm_lead);
		vim_free(smp->sm_to);
		vim_free(smp->sm_lead_w);
		vim_free(smp->sm_oneof_w);
		vim_free(smp->sm_to_w);
		return SP_OTHERERROR;
	    }
	}
#endif
    }

    if (gap->ga_len > 0)
    {
	/* Add one extra entry to mark the end with an empty sm_lead.  Avoids
	 * that we need to check the index every time. */
	smp = &((salitem_T *)gap->ga_data)[gap->ga_len];
	if ((p = alloc(1)) == NULL)
	    return SP_OTHERERROR;
	p[0] = NUL;
	smp->sm_lead = p;
	smp->sm_leadlen = 0;
	smp->sm_oneof = NULL;
	smp->sm_rules = p;
	smp->sm_to = NULL;
#ifdef FEAT_MBYTE
	if (has_mbyte)
	{
	    smp->sm_lead_w = mb_str2wide(smp->sm_lead);
	    smp->sm_leadlen = 0;
	    smp->sm_oneof_w = NULL;
	    smp->sm_to_w = NULL;
	}
#endif
	++gap->ga_len;
    }

    /* Fill the first-index table. */
    set_sal_first(slang);

    return 0;
}