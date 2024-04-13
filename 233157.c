set_spell_finish(spelltab_T *new_st)
{
    int		i;

    if (did_set_spelltab)
    {
	/* check that it's the same table */
	for (i = 0; i < 256; ++i)
	{
	    if (spelltab.st_isw[i] != new_st->st_isw[i]
		    || spelltab.st_isu[i] != new_st->st_isu[i]
		    || spelltab.st_fold[i] != new_st->st_fold[i]
		    || spelltab.st_upper[i] != new_st->st_upper[i])
	    {
		EMSG(_("E763: Word characters differ between spell files"));
		return FAIL;
	    }
	}
    }
    else
    {
	/* copy the new spelltab into the one being used */
	spelltab = *new_st;
	did_set_spelltab = TRUE;
    }

    return OK;
}