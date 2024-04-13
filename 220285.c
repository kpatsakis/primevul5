archive_mstring_get_mbs_l(struct archive_mstring *aes,
    const char **p, size_t *length, struct archive_string_conv *sc)
{
	int r, ret = 0;

#if defined(_WIN32) && !defined(__CYGWIN__)
	/*
	 * Internationalization programming on Windows must use Wide
	 * characters because Windows platform cannot make locale UTF-8.
	 */
	if (sc != NULL && (aes->aes_set & AES_SET_WCS) != 0) {
		archive_string_empty(&(aes->aes_mbs_in_locale));
		r = archive_string_append_from_wcs_in_codepage(
		    &(aes->aes_mbs_in_locale), aes->aes_wcs.s,
		    aes->aes_wcs.length, sc);
		if (r == 0) {
			*p = aes->aes_mbs_in_locale.s;
			if (length != NULL)
				*length = aes->aes_mbs_in_locale.length;
			return (0);
		} else if (errno == ENOMEM)
			return (-1);
		else
			ret = -1;
	}
#endif

	/* If there is not an MBS form but is a WCS form, try converting
	 * with the native locale to be used for translating it to specified
	 * character-set. */
	if ((aes->aes_set & AES_SET_MBS) == 0 &&
	    (aes->aes_set & AES_SET_WCS) != 0) {
		archive_string_empty(&(aes->aes_mbs));
		r = archive_string_append_from_wcs(&(aes->aes_mbs),
		    aes->aes_wcs.s, aes->aes_wcs.length);
		if (r == 0)
			aes->aes_set |= AES_SET_MBS;
		else if (errno == ENOMEM)
			return (-1);
		else
			ret = -1;
	}
	/* If we already have an MBS form, use it to be translated to
	 * specified character-set. */
	if (aes->aes_set & AES_SET_MBS) {
		if (sc == NULL) {
			/* Conversion is unneeded. */
			*p = aes->aes_mbs.s;
			if (length != NULL)
				*length = aes->aes_mbs.length;
			return (0);
		}
		ret = archive_strncpy_l(&(aes->aes_mbs_in_locale),
		    aes->aes_mbs.s, aes->aes_mbs.length, sc);
		*p = aes->aes_mbs_in_locale.s;
		if (length != NULL)
			*length = aes->aes_mbs_in_locale.length;
	} else {
		*p = NULL;
		if (length != NULL)
			*length = 0;
	}
	return (ret);
}