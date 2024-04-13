utf16nbytes(const void *_p, size_t n)
{
	size_t s;
	const char *p, *pp;

	if (_p == NULL)
		return (0);
	p = (const char *)_p;

	/* Like strlen(p), except won't examine positions beyond p[n]. */
	s = 0;
	pp = p;
	n >>= 1;
	while (s < n && (pp[0] || pp[1])) {
		pp += 2;
		s++;
	}
	return (s<<1);
}