archive_string_append_from_wcs_in_codepage(struct archive_string *as,
    const wchar_t *ws, size_t len, struct archive_string_conv *sc)
{
	BOOL defchar_used, *dp;
	int count, ret = 0;
	UINT to_cp;
	int wslen = (int)len;

	if (sc != NULL)
		to_cp = sc->to_cp;
	else
		to_cp = get_current_codepage();

	if (to_cp == CP_C_LOCALE) {
		/*
		 * "C" locale special processing.
		 */
		const wchar_t *wp = ws;
		char *p;

		if (NULL == archive_string_ensure(as,
		    as->length + wslen +1))
			return (-1);
		p = as->s + as->length;
		count = 0;
		defchar_used = 0;
		while (count < wslen && *wp) {
			if (*wp > 255) {
				*p++ = '?';
				wp++;
				defchar_used = 1;
			} else
				*p++ = (char)*wp++;
			count++;
		}
	} else if (sc != NULL && (sc->flag & SCONV_TO_UTF16)) {
		uint16_t *u16;

		if (NULL ==
		    archive_string_ensure(as, as->length + len * 2 + 2))
			return (-1);
		u16 = (uint16_t *)(as->s + as->length);
		count = 0;
		defchar_used = 0;
		if (sc->flag & SCONV_TO_UTF16BE) {
			while (count < (int)len && *ws) {
				archive_be16enc(u16+count, *ws);
				ws++;
				count++;
			}
		} else {
			while (count < (int)len && *ws) {
				archive_le16enc(u16+count, *ws);
				ws++;
				count++;
			}
		}
		count <<= 1; /* to be byte size */
	} else {
		/* Make sure the MBS buffer has plenty to set. */
		if (NULL ==
		    archive_string_ensure(as, as->length + len * 2 + 1))
			return (-1);
		do {
			defchar_used = 0;
			if (to_cp == CP_UTF8 || sc == NULL)
				dp = NULL;
			else
				dp = &defchar_used;
			count = WideCharToMultiByte(to_cp, 0, ws, wslen,
			    as->s + as->length, (int)as->buffer_length-1, NULL, dp);
			if (count == 0 &&
			    GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				/* Expand the MBS buffer and retry. */
				if (NULL == archive_string_ensure(as,
					as->buffer_length + len))
					return (-1);
				continue;
			}
			if (count == 0)
				ret = -1;
			break;
		} while (1);
	}
	as->length += count;
	as->s[as->length] = '\0';
	return (defchar_used?-1:ret);
}