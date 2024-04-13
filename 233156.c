get_pfxlist(
    afffile_T	*affile,
    char_u	*afflist,
    char_u	*store_afflist)
{
    char_u	*p;
    char_u	*prevp;
    int		cnt = 0;
    int		id;
    char_u	key[AH_KEY_LEN];
    hashitem_T	*hi;

    for (p = afflist; *p != NUL; )
    {
	prevp = p;
	if (get_affitem(affile->af_flagtype, &p) != 0)
	{
	    /* A flag is a postponed prefix flag if it appears in "af_pref"
	     * and it's ID is not zero. */
	    vim_strncpy(key, prevp, p - prevp);
	    hi = hash_find(&affile->af_pref, key);
	    if (!HASHITEM_EMPTY(hi))
	    {
		id = HI2AH(hi)->ah_newID;
		if (id != 0)
		    store_afflist[cnt++] = id;
	    }
	}
	if (affile->af_flagtype == AFT_NUM && *p == ',')
	    ++p;
    }

    store_afflist[cnt] = NUL;
    return cnt;
}