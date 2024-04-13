affitem2flag(
    int		flagtype,
    char_u	*item,
    char_u	*fname,
    int		lnum)
{
    unsigned	res;
    char_u	*p = item;

    res = get_affitem(flagtype, &p);
    if (res == 0)
    {
	if (flagtype == AFT_NUM)
	    smsg((char_u *)_("Flag is not a number in %s line %d: %s"),
							   fname, lnum, item);
	else
	    smsg((char_u *)_("Illegal flag in %s line %d: %s"),
							   fname, lnum, item);
    }
    if (*p != NUL)
    {
	smsg((char_u *)_(e_affname), fname, lnum, item);
	return 0;
    }

    return res;
}