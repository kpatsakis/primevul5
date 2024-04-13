strncat_in_codepage(struct archive_string *as,
    const void *_p, size_t length, struct archive_string_conv *sc)
{
	const char *s = (const char *)_p;
	struct archive_wstring aws;
	size_t l;
	int r, saved_flag;

	archive_string_init(&aws);
	saved_flag = sc->flag;
	sc->flag &= ~(SCONV_NORMALIZATION_D | SCONV_NORMALIZATION_C);
	r = archive_wstring_append_from_mbs_in_codepage(&aws, s, length, sc);
	sc->flag = saved_flag;
	if (r != 0) {
		archive_wstring_free(&aws);
		if (errno != ENOMEM)
			archive_string_append(as, s, length);
		return (-1);
	}

	l = as->length;
	r = archive_string_append_from_wcs_in_codepage(
	    as, aws.s, aws.length, sc);
	if (r != 0 && errno != ENOMEM && l == as->length)
		archive_string_append(as, s, length);
	archive_wstring_free(&aws);
	return (r);
}