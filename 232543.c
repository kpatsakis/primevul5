get_win_fio_flags(char_u *ptr)
{
    int		cp;

    /* Cannot do this when 'encoding' is not utf-8 and not a codepage. */
    if (!enc_utf8 && enc_codepage <= 0)
	return 0;

    cp = encname2codepage(ptr);
    if (cp == 0)
    {
#  ifdef CP_UTF8	/* VC 4.1 doesn't define CP_UTF8 */
	if (STRCMP(ptr, "utf-8") == 0)
	    cp = CP_UTF8;
	else
#  endif
	    return 0;
    }
    return FIO_PUT_CP(cp) | FIO_CODEPAGE;
}