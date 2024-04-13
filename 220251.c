best_effort_strncat_from_utf16(struct archive_string *as, const void *_p,
    size_t bytes, struct archive_string_conv *sc, int be)
{
	const char *utf16 = (const char *)_p;
	char *mbs;
	uint32_t uc;
	int n, ret;

	(void)sc; /* UNUSED */
	/*
	 * Other case, we should do the best effort.
	 * If all character are ASCII(<0x7f), we can convert it.
	 * if not , we set a alternative character and return -1.
	 */
	ret = 0;
	if (archive_string_ensure(as, as->length + bytes +1) == NULL)
		return (-1);
	mbs = as->s + as->length;

	while ((n = utf16_to_unicode(&uc, utf16, bytes, be)) != 0) {
		if (n < 0) {
			n *= -1;
			ret =  -1;
		}
		bytes -= n;
		utf16 += n;

		if (uc > 127) {
			/* We cannot handle it. */
			*mbs++ = '?';
			ret =  -1;
		} else
			*mbs++ = (char)uc;
	}
	as->length = mbs - as->s;
	as->s[as->length] = '\0';
	return (ret);
}