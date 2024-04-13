get_compflags(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist)
{
    char_u	*p;
    char_u	*prevp;
    int		cnt = 0;
    char_u	key[AH_KEY_LEN];
    hashitem_T	*hi;

    for (p = afflist; *p != NUL; )
    {
	prevp = p;
	if (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    /* A flag is a compound flag if it appears in "af_comp". */
	    vim_strncpy(key, prevp, p - prevp);
	    hi = hash_find(&affile->af_comp, key);
	    if (!HASHITEM_EMPTY(hi))
		store_afflist[cnt++] = HI2CI(hi)->ci_newID;
	}
	if (affile->af_flagtype == AFT_NUM && *p == ',')
	    ++p;
    }

    store_afflist[cnt] = NUL;
}