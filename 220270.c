invalid_mbs(const void *_p, size_t n, struct archive_string_conv *sc)
{
	const char *p = (const char *)_p;
	unsigned codepage;
	DWORD mbflag = MB_ERR_INVALID_CHARS;

	if (sc->flag & SCONV_FROM_CHARSET)
		codepage = sc->to_cp;
	else
		codepage = sc->from_cp;

	if (codepage == CP_C_LOCALE)
		return (0);
	if (codepage != CP_UTF8)
		mbflag |= MB_PRECOMPOSED;

	if (MultiByteToWideChar(codepage, mbflag, p, (int)n, NULL, 0) == 0)
		return (-1); /* Invalid */
	return (0); /* Okay */
}