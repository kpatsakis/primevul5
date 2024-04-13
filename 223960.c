vmod_get(VRT_CTX, VCL_HEADER hdr, VCL_REGEX re)
{
	struct http *hp;
	unsigned u;
	const char *p;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	AN(re);

	hp = VRT_selecthttp(ctx, hdr->where);
	u = header_http_findhdr(ctx, hp, hdr->what, re);
	if (u == 0)
		return (NULL);
	p = hp->hd[u].b + hdr->what[0];
	while (*p == ' ' || *p == '\t')
		p++;
	return (p);
}