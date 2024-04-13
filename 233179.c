set_map_str(slang_T *lp, char_u *map)
{
    char_u	*p;
    int		headc = 0;
    int		c;
    int		i;

    if (*map == NUL)
    {
	lp->sl_has_map = FALSE;
	return;
    }
    lp->sl_has_map = TRUE;

    /* Init the array and hash tables empty. */
    for (i = 0; i < 256; ++i)
	lp->sl_map_array[i] = 0;
#ifdef FEAT_MBYTE
    hash_init(&lp->sl_map_hash);
#endif

    /*
     * The similar characters are stored separated with slashes:
     * "aaa/bbb/ccc/".  Fill sl_map_array[c] with the character before c and
     * before the same slash.  For characters above 255 sl_map_hash is used.
     */
    for (p = map; *p != NUL; )
    {
#ifdef FEAT_MBYTE
	c = mb_cptr2char_adv(&p);
#else
	c = *p++;
#endif
	if (c == '/')
	    headc = 0;
	else
	{
	    if (headc == 0)
		 headc = c;

#ifdef FEAT_MBYTE
	    /* Characters above 255 don't fit in sl_map_array[], put them in
	     * the hash table.  Each entry is the char, a NUL the headchar and
	     * a NUL. */
	    if (c >= 256)
	    {
		int	    cl = mb_char2len(c);
		int	    headcl = mb_char2len(headc);
		char_u	    *b;
		hash_T	    hash;
		hashitem_T  *hi;

		b = alloc((unsigned)(cl + headcl + 2));
		if (b == NULL)
		    return;
		mb_char2bytes(c, b);
		b[cl] = NUL;
		mb_char2bytes(headc, b + cl + 1);
		b[cl + 1 + headcl] = NUL;
		hash = hash_hash(b);
		hi = hash_lookup(&lp->sl_map_hash, b, hash);
		if (HASHITEM_EMPTY(hi))
		    hash_add_item(&lp->sl_map_hash, hi, b, hash);
		else
		{
		    /* This should have been checked when generating the .spl
		     * file. */
		    EMSG(_("E783: duplicate char in MAP entry"));
		    vim_free(b);
		}
	    }
	    else
#endif
		lp->sl_map_array[c] = headc;
	}
    }
}