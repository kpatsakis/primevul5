aff_process_flags(afffile_T *affile, affentry_T *entry)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;

    if (entry->ae_flags != NULL
		&& (affile->af_compforbid != 0 || affile->af_comppermit != 0))
    {
	for (p = entry->ae_flags; *p != NUL; )
	{
	    prevp = p;
	    flag = get_affitem(affile->af_flagtype, &p);
	    if (flag == affile->af_comppermit || flag == affile->af_compforbid)
	    {
		STRMOVE(prevp, p);
		p = prevp;
		if (flag == affile->af_comppermit)
		    entry->ae_comppermit = TRUE;
		else
		    entry->ae_compforbid = TRUE;
	    }
	    if (affile->af_flagtype == AFT_NUM && *p == ',')
		++p;
	}
	if (*entry->ae_flags == NUL)
	    entry->ae_flags = NULL;	/* nothing left */
    }
}