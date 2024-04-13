mbsnbytes(const void *_p, size_t n)
{
	size_t s;
	const char *p, *pp;

	if (_p == NULL)
		return (0);
	p = (const char *)_p;

	/* Like strlen(p), except won't examine positions beyond p[n]. */
	s = 0;
	pp = p;
	while (s < n && *pp) {
		pp++;
		s++;
	}
	return (s);
}