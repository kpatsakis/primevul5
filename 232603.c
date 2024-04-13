get_fio_flags(char_u *ptr)
{
    int		prop;

    if (*ptr == NUL)
	ptr = p_enc;

    prop = enc_canon_props(ptr);
    if (prop & ENC_UNICODE)
    {
	if (prop & ENC_2BYTE)
	{
	    if (prop & ENC_ENDIAN_L)
		return FIO_UCS2 | FIO_ENDIAN_L;
	    return FIO_UCS2;
	}
	if (prop & ENC_4BYTE)
	{
	    if (prop & ENC_ENDIAN_L)
		return FIO_UCS4 | FIO_ENDIAN_L;
	    return FIO_UCS4;
	}
	if (prop & ENC_2WORD)
	{
	    if (prop & ENC_ENDIAN_L)
		return FIO_UTF16 | FIO_ENDIAN_L;
	    return FIO_UTF16;
	}
	return FIO_UTF8;
    }
    if (prop & ENC_LATIN1)
	return FIO_LATIN1;
    /* must be ENC_DBCS, requires iconv() */
    return 0;
}