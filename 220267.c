archive_mstring_get_mbs(struct archive *a, struct archive_mstring *aes,
    const char **p)
{
	int r, ret = 0;

	(void)a; /* UNUSED */
	/* If we already have an MBS form, return that immediately. */
	if (aes->aes_set & AES_SET_MBS) {
		*p = aes->aes_mbs.s;
		return (ret);
	}

	*p = NULL;
	/* If there's a WCS form, try converting with the native locale. */
	if (aes->aes_set & AES_SET_WCS) {
		archive_string_empty(&(aes->aes_mbs));
		r = archive_string_append_from_wcs(&(aes->aes_mbs),
		    aes->aes_wcs.s, aes->aes_wcs.length);
		*p = aes->aes_mbs.s;
		if (r == 0) {
			aes->aes_set |= AES_SET_MBS;
			return (ret);
		} else
			ret = -1;
	}

	/*
	 * Only a UTF-8 form cannot avail because its conversion already
	 * failed at archive_mstring_update_utf8().
	 */
	return (ret);
}