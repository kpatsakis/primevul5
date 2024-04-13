cesu8_to_unicode(uint32_t *pwc, const char *s, size_t n)
{
	uint32_t wc = 0;
	int cnt;

	cnt = _utf8_to_unicode(&wc, s, n);
	if (cnt == 3 && IS_HIGH_SURROGATE_LA(wc)) {
		uint32_t wc2 = 0;
		if (n - 3 < 3) {
			/* Invalid byte sequence. */
			goto invalid_sequence;
		}
		cnt = _utf8_to_unicode(&wc2, s+3, n-3);
		if (cnt != 3 || !IS_LOW_SURROGATE_LA(wc2)) {
			/* Invalid byte sequence. */
			goto invalid_sequence;
		}
		wc = combine_surrogate_pair(wc, wc2);
		cnt = 6;
	} else if (cnt == 3 && IS_LOW_SURROGATE_LA(wc)) {
		/* Invalid byte sequence. */
		goto invalid_sequence;
	}
	*pwc = wc;
	return (cnt);
invalid_sequence:
	*pwc = UNICODE_R_CHAR;/* set the Replacement Character instead. */
	if (cnt > 0)
		cnt *= -1;
	return (cnt);
}