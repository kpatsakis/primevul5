header_http_match(VRT_CTX, const struct http *hp, unsigned u, VCL_REGEX re,
    const char *hdr)
{
	const char *start;
	unsigned l;

	assert(hdr);
	assert(hp);

	Tcheck(hp->hd[u]);
	if (hp->hd[u].b == NULL)
		return (0);

	l = hdr[0];

	if (!header_http_IsHdr(&hp->hd[u], hdr))
		return (0);

	if (re == NULL)
		return (1);

	start = hp->hd[u].b + l;
	while (*start != '\0' && *start == ' ')
		start++;

	if (!*start)
		return (0);
	if (VRT_re_match(ctx, start, re))
		return (1);

	return (0);
}