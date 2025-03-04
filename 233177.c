set_spell_charflags(
    char_u	*flags,
    int		cnt,	    /* length of "flags" */
    char_u	*fol)
{
    /* We build the new tables here first, so that we can compare with the
     * previous one. */
    spelltab_T	new_st;
    int		i;
    char_u	*p = fol;
    int		c;

    clear_spell_chartab(&new_st);

    for (i = 0; i < 128; ++i)
    {
	if (i < cnt)
	{
	    new_st.st_isw[i + 128] = (flags[i] & CF_WORD) != 0;
	    new_st.st_isu[i + 128] = (flags[i] & CF_UPPER) != 0;
	}

	if (*p != NUL)
	{
#ifdef FEAT_MBYTE
	    c = mb_ptr2char_adv(&p);
#else
	    c = *p++;
#endif
	    new_st.st_fold[i + 128] = c;
	    if (i + 128 != c && new_st.st_isu[i + 128] && c < 256)
		new_st.st_upper[c] = i + 128;
	}
    }

    (void)set_spell_finish(&new_st);
}