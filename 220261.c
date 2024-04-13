win_strncat_to_utf16(struct archive_string *as16, const void *_p,
    size_t length, struct archive_string_conv *sc, int bigendian)
{
	const char *s = (const char *)_p;
	char *u16;
	size_t count, avail;

	if (archive_string_ensure(as16,
	    as16->length + (length + 1) * 2) == NULL)
		return (-1);

	u16 = as16->s + as16->length;
	avail = as16->buffer_length - 2;
	if (sc->from_cp == CP_C_LOCALE) {
		/*
		 * "C" locale special process.
		 */
		count = 0;
		while (count < length && *s) {
			if (bigendian)
				archive_be16enc(u16, *s);
			else
				archive_le16enc(u16, *s);
			u16 += 2;
			s++;
			count++;
		}
		as16->length += count << 1;
		as16->s[as16->length] = 0;
		as16->s[as16->length+1] = 0;
		return (0);
	}
	do {
		count = MultiByteToWideChar(sc->from_cp,
		    MB_PRECOMPOSED, s, (int)length, (LPWSTR)u16, (int)avail>>1);
		/* Exit loop if we succeeded */
		if (count != 0 ||
		    GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
			break;
		}
		/* Expand buffer and try again */
		count = MultiByteToWideChar(sc->from_cp,
		    MB_PRECOMPOSED, s, (int)length, NULL, 0);
		if (archive_string_ensure(as16, (count +1) * 2)
		    == NULL)
			return (-1);
		u16 = as16->s + as16->length;
		avail = as16->buffer_length - 2;
	} while (1);
	as16->length += count * 2;
	as16->s[as16->length] = 0;
	as16->s[as16->length+1] = 0;
	if (count == 0)
		return (-1);

	if (is_big_endian()) {
		if (!bigendian) {
			while (count > 0) {
				uint16_t v = archive_be16dec(u16);
				archive_le16enc(u16, v);
				u16 += 2;
				count--;
			}
		}
	} else {
		if (bigendian) {
			while (count > 0) {
				uint16_t v = archive_le16dec(u16);
				archive_be16enc(u16, v);
				u16 += 2;
				count--;
			}
		}
	}
	return (0);
}