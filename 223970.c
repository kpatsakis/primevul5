vmod_dyn(VRT_CTX, VCL_HTTP hp, VCL_STRING name)
{
	// usual assertions are in selectwhere()
	enum gethdr_e where = selectwhere(ctx, hp);
	char *what;
	const char *p;
	struct gethdr_s *hdr;
	size_t l;

	if (name == NULL || *name == '\0')
		return (&hdr_null[where]);

	p = strchr(name, ':');
	if (p != NULL)
		l = p - name;
	else
		l = strlen(name);

	assert(l <= CHAR_MAX);

	hdr = WS_Alloc(ctx->ws, sizeof *hdr);
	what = WS_Alloc(ctx->ws, l + 3);
	if (hdr == NULL || what == NULL) {
		VRT_fail(ctx, "out of workspace");
		// avoid null check in caller
		return (&hdr_null[where]);
	}

	what[0] = (char)l + 1;
	(void) strncpy(&what[1], name, l);
	what[l+1] = ':';
	what[l+2] = '\0';

	hdr->where = where;
	hdr->what = what;
	return (hdr);
}