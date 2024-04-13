archive_mstring_update_utf8(struct archive *a, struct archive_mstring *aes,
    const char *utf8)
{
	struct archive_string_conv *sc;
	int r;

	if (utf8 == NULL) {
		aes->aes_set = 0;
		return (0); /* Succeeded in clearing everything. */
	}

	/* Save the UTF8 string. */
	archive_strcpy(&(aes->aes_utf8), utf8);

	/* Empty the mbs and wcs strings. */
	archive_string_empty(&(aes->aes_mbs));
	archive_wstring_empty(&(aes->aes_wcs));

	aes->aes_set = AES_SET_UTF8;	/* Only UTF8 is set now. */

	/* Try converting UTF-8 to MBS, return false on failure. */
	sc = archive_string_conversion_from_charset(a, "UTF-8", 1);
	if (sc == NULL)
		return (-1);/* Couldn't allocate memory for sc. */
	r = archive_strcpy_l(&(aes->aes_mbs), utf8, sc);
	if (a == NULL)
		free_sconv_object(sc);
	if (r != 0)
		return (-1);
	aes->aes_set = AES_SET_UTF8 | AES_SET_MBS; /* Both UTF8 and MBS set. */

	/* Try converting MBS to WCS, return false on failure. */
	if (archive_wstring_append_from_mbs(&(aes->aes_wcs), aes->aes_mbs.s,
	    aes->aes_mbs.length))
		return (-1);
	aes->aes_set = AES_SET_UTF8 | AES_SET_WCS | AES_SET_MBS;

	/* All conversions succeeded. */
	return (0);
}