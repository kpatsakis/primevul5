transchar_nonprint(buf_T *buf, char_u *charbuf, int c)
{
    if (c == NL)
	c = NUL;		// we use newline in place of a NUL
    else if (c == CAR && get_fileformat(buf) == EOL_MAC)
	c = NL;			// we use CR in place of  NL in this case

    if (dy_flags & DY_UHEX)		// 'display' has "uhex"
	transchar_hex(charbuf, c);

#ifdef EBCDIC
    // For EBCDIC only the characters 0-63 and 255 are not printable
    else if (CtrlChar(c) != 0 || c == DEL)
#else
    else if (c <= 0x7f)			// 0x00 - 0x1f and 0x7f
#endif
    {
	charbuf[0] = '^';
#ifdef EBCDIC
	if (c == DEL)
	    charbuf[1] = '?';		// DEL displayed as ^?
	else
	    charbuf[1] = CtrlChar(c);
#else
	charbuf[1] = c ^ 0x40;		// DEL displayed as ^?
#endif

	charbuf[2] = NUL;
    }
    else if (enc_utf8 && c >= 0x80)
    {
	transchar_hex(charbuf, c);
    }
#ifndef EBCDIC
    else if (c >= ' ' + 0x80 && c <= '~' + 0x80)    // 0xa0 - 0xfe
    {
	charbuf[0] = '|';
	charbuf[1] = c - 0x80;
	charbuf[2] = NUL;
    }
#else
    else if (c < 64)
    {
	charbuf[0] = '~';
	charbuf[1] = MetaChar(c);
	charbuf[2] = NUL;
    }
#endif
    else					    // 0x80 - 0x9f and 0xff
    {
	/*
	 * TODO: EBCDIC I don't know what to do with this chars, so I display
	 * them as '~?' for now
	 */
	charbuf[0] = '~';
#ifdef EBCDIC
	charbuf[1] = '?';			// 0xff displayed as ~?
#else
	charbuf[1] = (c - 0x80) ^ 0x40;	// 0xff displayed as ~?
#endif
	charbuf[2] = NUL;
    }
}