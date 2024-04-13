need_conversion(char_u *fenc)
{
    int		same_encoding;
    int		enc_flags;
    int		fenc_flags;

    if (*fenc == NUL || STRCMP(p_enc, fenc) == 0)
    {
	same_encoding = TRUE;
	fenc_flags = 0;
    }
    else
    {
	/* Ignore difference between "ansi" and "latin1", "ucs-4" and
	 * "ucs-4be", etc. */
	enc_flags = get_fio_flags(p_enc);
	fenc_flags = get_fio_flags(fenc);
	same_encoding = (enc_flags != 0 && fenc_flags == enc_flags);
    }
    if (same_encoding)
    {
	/* Specified encoding matches with 'encoding'.  This requires
	 * conversion when 'encoding' is Unicode but not UTF-8. */
	return enc_unicode != 0;
    }

    /* Encodings differ.  However, conversion is not needed when 'enc' is any
     * Unicode encoding and the file is UTF-8. */
    return !(enc_utf8 && fenc_flags == FIO_UTF8);
}