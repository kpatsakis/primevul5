flag_in_afflist(int flagtype, char_u *afflist, unsigned flag)
{
    char_u	*p;
    unsigned	n;

    switch (flagtype)
    {
	case AFT_CHAR:
	    return vim_strchr(afflist, flag) != NULL;

	case AFT_CAPLONG:
	case AFT_LONG:
	    for (p = afflist; *p != NUL; )
	    {
#ifdef FEAT_MBYTE
		n = mb_ptr2char_adv(&p);
#else
		n = *p++;
#endif
		if ((flagtype == AFT_LONG || (n >= 'A' && n <= 'Z'))
								 && *p != NUL)
#ifdef FEAT_MBYTE
		    n = mb_ptr2char_adv(&p) + (n << 16);
#else
		    n = *p++ + (n << 16);
#endif
		if (n == flag)
		    return TRUE;
	    }
	    break;

	case AFT_NUM:
	    for (p = afflist; *p != NUL; )
	    {
		n = getdigits(&p);
		if (n == flag)
		    return TRUE;
		if (*p != NUL)	/* skip over comma */
		    ++p;
	    }
	    break;
    }
    return FALSE;
}