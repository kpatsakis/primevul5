utf8_to_unicode(uint32_t *pwc, const char *s, size_t n)
{
	int cnt;

	cnt = _utf8_to_unicode(pwc, s, n);
	/* Any of Surrogate pair is not legal Unicode values. */
	if (cnt == 3 && IS_SURROGATE_PAIR_LA(*pwc))
		return (-3);
	return (cnt);
}