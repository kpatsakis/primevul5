archive_wstring_append_from_mbs_in_codepage(struct archive_wstring *dest,
    const char *s, size_t length, struct archive_string_conv *sc)
{
	int count, ret = 0;
	UINT from_cp;

	if (sc != NULL)
		from_cp = sc->from_cp;
	else
		from_cp = get_current_codepage();

	if (from_cp == CP_C_LOCALE) {
		/*
		 * "C" locale special processing.
		 */
		wchar_t *ws;
		const unsigned char *mp;

		if (NULL == archive_wstring_ensure(dest,
		    dest->length + length + 1))
			return (-1);

		ws = dest->s + dest->length;
		mp = (const unsigned char *)s;
		count = 0;
		while (count < (int)length && *mp) {
			*ws++ = (wchar_t)*mp++;
			count++;
		}
	} else if (sc != NULL &&
	    (sc->flag & (SCONV_NORMALIZATION_C | SCONV_NORMALIZATION_D))) {
		/*
		 * Normalize UTF-8 and UTF-16BE and convert it directly
		 * to UTF-16 as wchar_t.
		 */
		struct archive_string u16;
		int saved_flag = sc->flag;/* save current flag. */

		if (is_big_endian())
			sc->flag |= SCONV_TO_UTF16BE;
		else
			sc->flag |= SCONV_TO_UTF16LE;

		if (sc->flag & SCONV_FROM_UTF16) {
			/*
			 *  UTF-16BE/LE NFD ===> UTF-16 NFC
			 *  UTF-16BE/LE NFC ===> UTF-16 NFD
			 */
			count = (int)utf16nbytes(s, length);
		} else {
			/*
			 *  UTF-8 NFD ===> UTF-16 NFC
			 *  UTF-8 NFC ===> UTF-16 NFD
			 */
			count = (int)mbsnbytes(s, length);
		}
		u16.s = (char *)dest->s;
		u16.length = dest->length << 1;;
		u16.buffer_length = dest->buffer_length;
		if (sc->flag & SCONV_NORMALIZATION_C)
			ret = archive_string_normalize_C(&u16, s, count, sc);
		else
			ret = archive_string_normalize_D(&u16, s, count, sc);
		dest->s = (wchar_t *)u16.s;
		dest->length = u16.length >> 1;
		dest->buffer_length = u16.buffer_length;
		sc->flag = saved_flag;/* restore the saved flag. */
		return (ret);
	} else if (sc != NULL && (sc->flag & SCONV_FROM_UTF16)) {
		count = (int)utf16nbytes(s, length);
		count >>= 1; /* to be WCS length */
		/* Allocate memory for WCS. */
		if (NULL == archive_wstring_ensure(dest,
		    dest->length + count + 1))
			return (-1);
		wmemcpy(dest->s + dest->length, (const wchar_t *)s, count);
		if ((sc->flag & SCONV_FROM_UTF16BE) && !is_big_endian()) {
			uint16_t *u16 = (uint16_t *)(dest->s + dest->length);
			int b;
			for (b = 0; b < count; b++) {
				uint16_t val = archive_le16dec(u16+b);
				archive_be16enc(u16+b, val);
			}
		} else if ((sc->flag & SCONV_FROM_UTF16LE) && is_big_endian()) {
			uint16_t *u16 = (uint16_t *)(dest->s + dest->length);
			int b;
			for (b = 0; b < count; b++) {
				uint16_t val = archive_be16dec(u16+b);
				archive_le16enc(u16+b, val);
			}
		}
	} else {
		DWORD mbflag;
		size_t buffsize;

		if (sc == NULL)
			mbflag = 0;
		else if (sc->flag & SCONV_FROM_CHARSET) {
			/* Do not trust the length which comes from
			 * an archive file. */
			length = mbsnbytes(s, length);
			mbflag = 0;
		} else
			mbflag = MB_PRECOMPOSED;

		buffsize = dest->length + length + 1;
		do {
			/* Allocate memory for WCS. */
			if (NULL == archive_wstring_ensure(dest, buffsize))
				return (-1);
			/* Convert MBS to WCS. */
			count = MultiByteToWideChar(from_cp,
			    mbflag, s, (int)length, dest->s + dest->length,
			    (int)(dest->buffer_length >> 1) -1);
			if (count == 0 &&
			    GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				/* Expand the WCS buffer. */
				buffsize = dest->buffer_length << 1;
				continue;
			}
			if (count == 0 && length != 0)
				ret = -1;
			break;
		} while (1);
	}
	dest->length += count;
	dest->s[dest->length] = L'\0';
	return (ret);
}