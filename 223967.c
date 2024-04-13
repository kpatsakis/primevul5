vmod_regsub(VRT_CTX, VCL_HTTP hp, VCL_REGEX re,
    VCL_STRING sub, VCL_BOOL all)
{
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);
	AN(re);

	for (unsigned u = HTTP_HDR_FIRST; u < hp->nhd; u++) {
		const char *hdr;
		VCL_STRING rewrite;

		Tcheck(hp->hd[u]);
		hdr = hp->hd[u].b;
		if (!VRT_re_match(ctx, hdr, re))
			continue;
		rewrite = VRT_regsub(ctx, all, hdr, re, sub);
		if (rewrite == hdr)
			continue;
		http_VSLH_del(hp, u);
		hp->hd[u].b = rewrite;
		hp->hd[u].e = strchr(rewrite, '\0');
		http_VSLH(hp, u);
	}
}