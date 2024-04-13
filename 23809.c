char *cgit_currenturl(void)
{
	if (!ctx.qry.url)
		return xstrdup(cgit_rooturl());
	const char *root = cgit_rooturl();
	size_t len = strlen(root);
	if (len && root[len - 1] == '/')
		return fmtalloc("%s%s", root, ctx.qry.url);
	return fmtalloc("%s/%s", root, ctx.qry.url);
}