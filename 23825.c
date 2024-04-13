static const char *hc(const char *page)
{
	if (!ctx.qry.page)
		return NULL;

	return strcmp(ctx.qry.page, page) ? NULL : "active";
}