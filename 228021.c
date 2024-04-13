stuffescaped(char_u *arg, int literally)
{
    int		c;
    char_u	*start;

    while (*arg != NUL)
    {
	// Stuff a sequence of normal ASCII characters, that's fast.  Also
	// stuff K_SPECIAL to get the effect of a special key when "literally"
	// is TRUE.
	start = arg;
	while ((*arg >= ' '
#ifndef EBCDIC
		    && *arg < DEL // EBCDIC: chars above space are normal
#endif
		    )
		|| (*arg == K_SPECIAL && !literally))
	    ++arg;
	if (arg > start)
	    stuffReadbuffLen(start, (long)(arg - start));

	// stuff a single special character
	if (*arg != NUL)
	{
	    if (has_mbyte)
		c = mb_cptr2char_adv(&arg);
	    else
		c = *arg++;
	    if (literally && ((c < ' ' && c != TAB) || c == DEL))
		stuffcharReadbuff(Ctrl_V);
	    stuffcharReadbuff(c);
	}
    }
}