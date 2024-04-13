process_compflags(
    spellinfo_T	*spin,
    afffile_T	*aff,
    char_u	*compflags)
{
    char_u	*p;
    char_u	*prevp;
    unsigned	flag;
    compitem_T	*ci;
    int		id;
    int		len;
    char_u	*tp;
    char_u	key[AH_KEY_LEN];
    hashitem_T	*hi;

    /* Make room for the old and the new compflags, concatenated with a / in
     * between.  Processing it makes it shorter, but we don't know by how
     * much, thus allocate the maximum. */
    len = (int)STRLEN(compflags) + 1;
    if (spin->si_compflags != NULL)
	len += (int)STRLEN(spin->si_compflags) + 1;
    p = getroom(spin, len, FALSE);
    if (p == NULL)
	return;
    if (spin->si_compflags != NULL)
    {
	STRCPY(p, spin->si_compflags);
	STRCAT(p, "/");
    }
    spin->si_compflags = p;
    tp = p + STRLEN(p);

    for (p = compflags; *p != NUL; )
    {
	if (vim_strchr((char_u *)"/?*+[]", *p) != NULL)
	    /* Copy non-flag characters directly. */
	    *tp++ = *p++;
	else
	{
	    /* First get the flag number, also checks validity. */
	    prevp = p;
	    flag = get_affitem(aff->af_flagtype, &p);
	    if (flag != 0)
	    {
		/* Find the flag in the hashtable.  If it was used before, use
		 * the existing ID.  Otherwise add a new entry. */
		vim_strncpy(key, prevp, p - prevp);
		hi = hash_find(&aff->af_comp, key);
		if (!HASHITEM_EMPTY(hi))
		    id = HI2CI(hi)->ci_newID;
		else
		{
		    ci = (compitem_T *)getroom(spin, sizeof(compitem_T), TRUE);
		    if (ci == NULL)
			break;
		    STRCPY(ci->ci_key, key);
		    ci->ci_flag = flag;
		    /* Avoid using a flag ID that has a special meaning in a
		     * regexp (also inside []). */
		    do
		    {
			check_renumber(spin);
			id = spin->si_newcompID--;
		    } while (vim_strchr((char_u *)"/?*+[]\\-^", id) != NULL);
		    ci->ci_newID = id;
		    hash_add(&aff->af_comp, ci->ci_key);
		}
		*tp++ = id;
	    }
	    if (aff->af_flagtype == AFT_NUM && *p == ',')
		++p;
	}
    }

    *tp = NUL;
}