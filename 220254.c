get_sconv_object(struct archive *a, const char *fc, const char *tc, int flag)
{
	struct archive_string_conv *sc;
	unsigned current_codepage;

	/* Check if we have made the sconv object. */
	sc = find_sconv_object(a, fc, tc);
	if (sc != NULL)
		return (sc);

	if (a == NULL)
		current_codepage = get_current_codepage();
	else
		current_codepage = a->current_codepage;

	sc = create_sconv_object(canonical_charset_name(fc),
	    canonical_charset_name(tc), current_codepage, flag);
	if (sc == NULL) {
		if (a != NULL)
			archive_set_error(a, ENOMEM,
			    "Could not allocate memory for "
			    "a string conversion object");
		return (NULL);
	}

	/*
	 * If there is no converter for current string conversion object,
	 * we cannot handle this conversion.
	 */
	if (sc->nconverter == 0) {
		if (a != NULL) {
#if HAVE_ICONV
			archive_set_error(a, ARCHIVE_ERRNO_MISC,
			    "iconv_open failed : Cannot handle ``%s''",
			    (flag & SCONV_TO_CHARSET)?tc:fc);
#else
			archive_set_error(a, ARCHIVE_ERRNO_MISC,
			    "A character-set conversion not fully supported "
			    "on this platform");
#endif
		}
		/* Failed; free a sconv object. */
		free_sconv_object(sc);
		return (NULL);
	}

	/*
	 * Success!
	 */
	if (a != NULL)
		add_sconv_object(a, sc);
	return (sc);
}