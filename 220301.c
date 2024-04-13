archive_wstring_append_from_mbs(struct archive_wstring *dest,
    const char *p, size_t len)
{
	size_t r;
	int ret_val = 0;
	/*
	 * No single byte will be more than one wide character,
	 * so this length estimate will always be big enough.
	 */
	// size_t wcs_length = len;
	size_t mbs_length = len;
	const char *mbs = p;
	wchar_t *wcs;
#if HAVE_MBRTOWC
	mbstate_t shift_state;

	memset(&shift_state, 0, sizeof(shift_state));
#endif
	/*
	 * As we decided to have wcs_length == mbs_length == len
	 * we can use len here instead of wcs_length
	 */
	if (NULL == archive_wstring_ensure(dest, dest->length + len + 1))
		return (-1);
	wcs = dest->s + dest->length;
	/*
	 * We cannot use mbsrtowcs/mbstowcs here because those may convert
	 * extra MBS when strlen(p) > len and one wide character consists of
	 * multi bytes.
	 */
	while (*mbs && mbs_length > 0) {
		/*
		 * The buffer we allocated is always big enough.
		 * Keep this code path in a comment if we decide to choose
		 * smaller wcs_length in the future
		 */
/*
		if (wcs_length == 0) {
			dest->length = wcs - dest->s;
			dest->s[dest->length] = L'\0';
			wcs_length = mbs_length;
			if (NULL == archive_wstring_ensure(dest,
			    dest->length + wcs_length + 1))
				return (-1);
			wcs = dest->s + dest->length;
		}
*/
#if HAVE_MBRTOWC
		r = mbrtowc(wcs, mbs, mbs_length, &shift_state);
#else
		r = mbtowc(wcs, mbs, mbs_length);
#endif
		if (r == (size_t)-1 || r == (size_t)-2) {
			ret_val = -1;
			break;
		}
		if (r == 0 || r > mbs_length)
			break;
		wcs++;
		// wcs_length--;
		mbs += r;
		mbs_length -= r;
	}
	dest->length = wcs - dest->s;
	dest->s[dest->length] = L'\0';
	return (ret_val);
}