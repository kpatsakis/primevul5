win_strncat_from_utf16(struct archive_string *as, const void *_p, size_t bytes,
    struct archive_string_conv *sc, int be)
{
	struct archive_string tmp;
	const char *u16;
	int ll;
	BOOL defchar;
	char *mbs;
	size_t mbs_size, b;
	int ret = 0;

	bytes &= ~1;
	if (archive_string_ensure(as, as->length + bytes +1) == NULL)
		return (-1);

	mbs = as->s + as->length;
	mbs_size = as->buffer_length - as->length -1;

	if (sc->to_cp == CP_C_LOCALE) {
		/*
		 * "C" locale special process.
		 */
		u16 = _p;
		ll = 0;
		for (b = 0; b < bytes; b += 2) {
			uint16_t val;
			if (be)
				val = archive_be16dec(u16+b);
			else
				val = archive_le16dec(u16+b);
			if (val > 255) {
				*mbs++ = '?';
				ret = -1;
			} else
				*mbs++ = (char)(val&0xff);
			ll++;
		}
		as->length += ll;
		as->s[as->length] = '\0';
		return (ret);
	}

	archive_string_init(&tmp);
	if (be) {
		if (is_big_endian()) {
			u16 = _p;
		} else {
			if (archive_string_ensure(&tmp, bytes+2) == NULL)
				return (-1);
			memcpy(tmp.s, _p, bytes);
			for (b = 0; b < bytes; b += 2) {
				uint16_t val = archive_be16dec(tmp.s+b);
				archive_le16enc(tmp.s+b, val);
			}
			u16 = tmp.s;
		}
	} else {
		if (!is_big_endian()) {
			u16 = _p;
		} else {
			if (archive_string_ensure(&tmp, bytes+2) == NULL)
				return (-1);
			memcpy(tmp.s, _p, bytes);
			for (b = 0; b < bytes; b += 2) {
				uint16_t val = archive_le16dec(tmp.s+b);
				archive_be16enc(tmp.s+b, val);
			}
			u16 = tmp.s;
		}
	}

	do {
		defchar = 0;
		ll = WideCharToMultiByte(sc->to_cp, 0,
		    (LPCWSTR)u16, (int)bytes>>1, mbs, (int)mbs_size,
			NULL, &defchar);
		/* Exit loop if we succeeded */
		if (ll != 0 ||
		    GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
			break;
		}
		/* Else expand buffer and loop to try again. */
		ll = WideCharToMultiByte(sc->to_cp, 0,
		    (LPCWSTR)u16, (int)bytes, NULL, 0, NULL, NULL);
		if (archive_string_ensure(as, ll +1) == NULL)
			return (-1);
		mbs = as->s + as->length;
		mbs_size = as->buffer_length - as->length -1;
	} while (1);
	archive_string_free(&tmp);
	as->length += ll;
	as->s[as->length] = '\0';
	if (ll == 0 || defchar)
		ret = -1;
	return (ret);
}