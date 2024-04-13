header_http_findhdr(VRT_CTX, const struct http *hp, const char *hdr, VCL_REGEX re)
{
        unsigned u;

        for (u = HTTP_HDR_FIRST; u < hp->nhd; u++) {
		if (header_http_match(ctx, hp, u, re, hdr))
			return (u);
        }
        return (0);
}