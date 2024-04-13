add_char_buff(buffheader_T *buf, int c)
{
    char_u	bytes[MB_MAXBYTES + 1];
    int		len;
    int		i;
    char_u	temp[4];

    if (IS_SPECIAL(c))
	len = 1;
    else
	len = (*mb_char2bytes)(c, bytes);
    for (i = 0; i < len; ++i)
    {
	if (!IS_SPECIAL(c))
	    c = bytes[i];

	if (IS_SPECIAL(c) || c == K_SPECIAL || c == NUL)
	{
	    // translate special key code into three byte sequence
	    temp[0] = K_SPECIAL;
	    temp[1] = K_SECOND(c);
	    temp[2] = K_THIRD(c);
	    temp[3] = NUL;
	}
#ifdef FEAT_GUI
	else if (c == CSI)
	{
	    // Translate a CSI to a CSI - KS_EXTRA - KE_CSI sequence
	    temp[0] = CSI;
	    temp[1] = KS_EXTRA;
	    temp[2] = (int)KE_CSI;
	    temp[3] = NUL;
	}
#endif
	else
	{
	    temp[0] = c;
	    temp[1] = NUL;
	}
	add_buff(buf, temp, -1L);
    }
}