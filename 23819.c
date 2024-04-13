char *cgit_repourl(const char *reponame)
{
	if (ctx.cfg.virtual_root)
		return fmtalloc("%s%s/", ctx.cfg.virtual_root, reponame);
	else
		return fmtalloc("?r=%s", reponame);
}