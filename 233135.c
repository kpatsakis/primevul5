set_spell_chartab(char_u *fol, char_u *low, char_u *upp)
{
    /* We build the new tables here first, so that we can compare with the
     * previous one. */
    spelltab_T	new_st;
    char_u	*pf = fol, *pl = low, *pu = upp;
    int		f, l, u;

    clear_spell_chartab(&new_st);

    while (*pf != NUL)
    {
	if (*pl == NUL || *pu == NUL)
	{
	    EMSG(_(e_affform));
	    return FAIL;
	}
#ifdef FEAT_MBYTE
	f = mb_ptr2char_adv(&pf);
	l = mb_ptr2char_adv(&pl);
	u = mb_ptr2char_adv(&pu);
#else
	f = *pf++;
	l = *pl++;
	u = *pu++;
#endif
	/* Every character that appears is a word character. */
	if (f < 256)
	    new_st.st_isw[f] = TRUE;
	if (l < 256)
	    new_st.st_isw[l] = TRUE;
	if (u < 256)
	    new_st.st_isw[u] = TRUE;

	/* if "LOW" and "FOL" are not the same the "LOW" char needs
	 * case-folding */
	if (l < 256 && l != f)
	{
	    if (f >= 256)
	    {
		EMSG(_(e_affrange));
		return FAIL;
	    }
	    new_st.st_fold[l] = f;
	}

	/* if "UPP" and "FOL" are not the same the "UPP" char needs
	 * case-folding, it's upper case and the "UPP" is the upper case of
	 * "FOL" . */
	if (u < 256 && u != f)
	{
	    if (f >= 256)
	    {
		EMSG(_(e_affrange));
		return FAIL;
	    }
	    new_st.st_fold[u] = f;
	    new_st.st_isu[u] = TRUE;
	    new_st.st_upper[f] = u;
	}
    }

    if (*pl != NUL || *pu != NUL)
    {
	EMSG(_(e_affform));
	return FAIL;
    }

    return set_spell_finish(&new_st);
}