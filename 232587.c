make_bom(char_u *buf, char_u *name)
{
    int		flags;
    char_u	*p;

    flags = get_fio_flags(name);

    /* Can't put a BOM in a non-Unicode file. */
    if (flags == FIO_LATIN1 || flags == 0)
	return 0;

    if (flags == FIO_UTF8)	/* UTF-8 */
    {
	buf[0] = 0xef;
	buf[1] = 0xbb;
	buf[2] = 0xbf;
	return 3;
    }
    p = buf;
    (void)ucs2bytes(0xfeff, &p, flags);
    return (int)(p - buf);
}