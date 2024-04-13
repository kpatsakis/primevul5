static const char *xquote(const char *str, const char *quote_chars)
{
	const char *q = quote(str, quote_chars);
	if (q == NULL) {
		fprintf(stderr, "%s: %s\n", progname, strerror(errno));
		exit(1);
	}
	return q;
}