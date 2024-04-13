header_http_cphdr(VRT_CTX, const struct http *hp, const char *hdr,
    VCL_HEADER dst)
{
        unsigned u;
	const char *p;
	struct strands s;

	s.n = 1;

        for (u = HTTP_HDR_FIRST; u < hp->nhd; u++) {
		if (!header_http_match(ctx, hp, u, NULL, hdr))
			continue;

		p = hp->hd[u].b + hdr[0];
		while (*p == ' ' || *p == '\t')
			p++;
		s.p = &p;
                vmod_append(ctx, dst, &s);
        }
}