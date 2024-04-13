iconv_strncat_in_locale(struct archive_string *as, const void *_p,
    size_t length, struct archive_string_conv *sc)
{
	ICONV_CONST char *itp;
	size_t remaining;
	iconv_t cd;
	char *outp;
	size_t avail, bs;
	int return_value = 0; /* success */
	int to_size, from_size;

	if (sc->flag & SCONV_TO_UTF16)
		to_size = 2;
	else
		to_size = 1;
	if (sc->flag & SCONV_FROM_UTF16)
		from_size = 2;
	else
		from_size = 1;

	if (archive_string_ensure(as, as->length + length*2+to_size) == NULL)
		return (-1);

	cd = sc->cd;
	itp = (char *)(uintptr_t)_p;
	remaining = length;
	outp = as->s + as->length;
	avail = as->buffer_length - as->length - to_size;
	while (remaining >= (size_t)from_size) {
		size_t result = iconv(cd, &itp, &remaining, &outp, &avail);

		if (result != (size_t)-1)
			break; /* Conversion completed. */

		if (errno == EILSEQ || errno == EINVAL) {
			/*
		 	 * If an output charset is UTF-8 or UTF-16BE/LE,
			 * unknown character should be U+FFFD
			 * (replacement character).
			 */
			if (sc->flag & (SCONV_TO_UTF8 | SCONV_TO_UTF16)) {
				size_t rbytes;
				if (sc->flag & SCONV_TO_UTF8)
					rbytes = sizeof(utf8_replacement_char);
				else
					rbytes = 2;

				if (avail < rbytes) {
					as->length = outp - as->s;
					bs = as->buffer_length +
					    (remaining * to_size) + rbytes;
					if (NULL ==
					    archive_string_ensure(as, bs))
						return (-1);
					outp = as->s + as->length;
					avail = as->buffer_length
					    - as->length - to_size;
				}
				if (sc->flag & SCONV_TO_UTF8)
					memcpy(outp, utf8_replacement_char, sizeof(utf8_replacement_char));
				else if (sc->flag & SCONV_TO_UTF16BE)
					archive_be16enc(outp, UNICODE_R_CHAR);
				else
					archive_le16enc(outp, UNICODE_R_CHAR);
				outp += rbytes;
				avail -= rbytes;
			} else {
				/* Skip the illegal input bytes. */
				*outp++ = '?';
				avail--;
			}
			itp += from_size;
			remaining -= from_size;
			return_value = -1; /* failure */
		} else {
			/* E2BIG no output buffer,
			 * Increase an output buffer.  */
			as->length = outp - as->s;
			bs = as->buffer_length + remaining * 2;
			if (NULL == archive_string_ensure(as, bs))
				return (-1);
			outp = as->s + as->length;
			avail = as->buffer_length - as->length - to_size;
		}
	}
	as->length = outp - as->s;
	as->s[as->length] = 0;
	if (to_size == 2)
		as->s[as->length+1] = 0;
	return (return_value);
}