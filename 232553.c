ucs2bytes(
    unsigned	c,		/* in: character */
    char_u	**pp,		/* in/out: pointer to result */
    int		flags)		/* FIO_ flags */
{
    char_u	*p = *pp;
    int		error = FALSE;
    int		cc;


    if (flags & FIO_UCS4)
    {
	if (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	    *p++ = (c >> 16);
	    *p++ = (c >> 24);
	}
	else
	{
	    *p++ = (c >> 24);
	    *p++ = (c >> 16);
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    else if (flags & (FIO_UCS2 | FIO_UTF16))
    {
	if (c >= 0x10000)
	{
	    if (flags & FIO_UTF16)
	    {
		/* Make two words, ten bits of the character in each.  First
		 * word is 0xd800 - 0xdbff, second one 0xdc00 - 0xdfff */
		c -= 0x10000;
		if (c >= 0x100000)
		    error = TRUE;
		cc = ((c >> 10) & 0x3ff) + 0xd800;
		if (flags & FIO_ENDIAN_L)
		{
		    *p++ = cc;
		    *p++ = ((unsigned)cc >> 8);
		}
		else
		{
		    *p++ = ((unsigned)cc >> 8);
		    *p++ = cc;
		}
		c = (c & 0x3ff) + 0xdc00;
	    }
	    else
		error = TRUE;
	}
	if (flags & FIO_ENDIAN_L)
	{
	    *p++ = c;
	    *p++ = (c >> 8);
	}
	else
	{
	    *p++ = (c >> 8);
	    *p++ = c;
	}
    }
    else    /* Latin1 */
    {
	if (c >= 0x100)
	{
	    error = TRUE;
	    *p++ = 0xBF;
	}
	else
	    *p++ = c;
    }

    *pp = p;
    return error;
}