AppendToRedobuffLit(
    char_u	*str,
    int		len)	    // length of "str" or -1 for up to the NUL
{
    char_u	*s = str;
    int		c;
    char_u	*start;

    if (block_redo)
	return;

    while (len < 0 ? *s != NUL : s - str < len)
    {
	// Put a string of normal characters in the redo buffer (that's
	// faster).
	start = s;
	while (*s >= ' '
#ifndef EBCDIC
		&& *s < DEL	// EBCDIC: all chars above space are normal
#endif
		&& (len < 0 || s - str < len))
	    ++s;

	// Don't put '0' or '^' as last character, just in case a CTRL-D is
	// typed next.
	if (*s == NUL && (s[-1] == '0' || s[-1] == '^'))
	    --s;
	if (s > start)
	    add_buff(&redobuff, start, (long)(s - start));

	if (*s == NUL || (len >= 0 && s - str >= len))
	    break;

	// Handle a special or multibyte character.
	if (has_mbyte)
	    // Handle composing chars separately.
	    c = mb_cptr2char_adv(&s);
	else
	    c = *s++;
	if (c < ' ' || c == DEL || (*s == NUL && (c == '0' || c == '^')))
	    add_char_buff(&redobuff, Ctrl_V);

	// CTRL-V '0' must be inserted as CTRL-V 048 (EBCDIC: xf0)
	if (*s == NUL && c == '0')
#ifdef EBCDIC
	    add_buff(&redobuff, (char_u *)"xf0", 3L);
#else
	    add_buff(&redobuff, (char_u *)"048", 3L);
#endif
	else
	    add_char_buff(&redobuff, c);
    }
}