read_redo(int init, int old_redo)
{
    static buffblock_T	*bp;
    static char_u	*p;
    int			c;
    int			n;
    char_u		buf[MB_MAXBYTES + 1];
    int			i;

    if (init)
    {
	if (old_redo)
	    bp = old_redobuff.bh_first.b_next;
	else
	    bp = redobuff.bh_first.b_next;
	if (bp == NULL)
	    return FAIL;
	p = bp->b_str;
	return OK;
    }
    if ((c = *p) != NUL)
    {
	// Reverse the conversion done by add_char_buff()
	// For a multi-byte character get all the bytes and return the
	// converted character.
	if (has_mbyte && (c != K_SPECIAL || p[1] == KS_SPECIAL))
	    n = MB_BYTE2LEN_CHECK(c);
	else
	    n = 1;
	for (i = 0; ; ++i)
	{
	    if (c == K_SPECIAL) // special key or escaped K_SPECIAL
	    {
		c = TO_SPECIAL(p[1], p[2]);
		p += 2;
	    }
#ifdef FEAT_GUI
	    if (c == CSI)	// escaped CSI
		p += 2;
#endif
	    if (*++p == NUL && bp->b_next != NULL)
	    {
		bp = bp->b_next;
		p = bp->b_str;
	    }
	    buf[i] = c;
	    if (i == n - 1)	// last byte of a character
	    {
		if (n != 1)
		    c = (*mb_ptr2char)(buf);
		break;
	    }
	    c = *p;
	    if (c == NUL)	// cannot happen?
		break;
	}
    }

    return c;
}