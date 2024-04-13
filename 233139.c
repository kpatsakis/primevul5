get_affitem(int flagtype, char_u **pp)
{
    int		res;

    if (flagtype == AFT_NUM)
    {
	if (!VIM_ISDIGIT(**pp))
	{
	    ++*pp;	/* always advance, avoid getting stuck */
	    return 0;
	}
	res = getdigits(pp);
    }
    else
    {
#ifdef FEAT_MBYTE
	res = mb_ptr2char_adv(pp);
#else
	res = *(*pp)++;
#endif
	if (flagtype == AFT_LONG || (flagtype == AFT_CAPLONG
						 && res >= 'A' && res <= 'Z'))
	{
	    if (**pp == NUL)
		return 0;
#ifdef FEAT_MBYTE
	    res = mb_ptr2char_adv(pp) + (res << 16);
#else
	    res = *(*pp)++ + (res << 16);
#endif
	}
    }
    return res;
}