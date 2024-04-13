best_effort_strncat_in_locale(struct archive_string *as, const void *_p,
    size_t length, struct archive_string_conv *sc)
{
	size_t remaining;
	const uint8_t *itp;
	int return_value = 0; /* success */

	/*
	 * If both from-locale and to-locale is the same, this makes a copy.
	 * And then this checks all copied MBS can be WCS if so returns 0.
	 */
	if (sc->same) {
		if (archive_string_append(as, _p, length) == NULL)
			return (-1);/* No memory */
		return (invalid_mbs(_p, length, sc));
	}

	/*
	 * If a character is ASCII, this just copies it. If not, this
	 * assigns '?' character instead but in UTF-8 locale this assigns
	 * byte sequence 0xEF 0xBD 0xBD, which are code point U+FFFD,
	 * a Replacement Character in Unicode.
	 */

	remaining = length;
	itp = (const uint8_t *)_p;
	while (*itp && remaining > 0) {
		if (*itp > 127) {
			// Non-ASCII: Substitute with suitable replacement
			if (sc->flag & SCONV_TO_UTF8) {
				if (archive_string_append(as, utf8_replacement_char, sizeof(utf8_replacement_char)) == NULL) {
					__archive_errx(1, "Out of memory");
				}
			} else {
				archive_strappend_char(as, '?');
			}
			return_value = -1;
		} else {
			archive_strappend_char(as, *itp);
		}
		++itp;
	}
	return (return_value);
}