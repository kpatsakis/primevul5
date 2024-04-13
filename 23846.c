char *cgit_fileurl(const char *reponame, const char *pagename,
		   const char *filename, const char *query)
{
	struct strbuf sb = STRBUF_INIT;
	char *delim;

	if (ctx.cfg.virtual_root) {
		strbuf_addf(&sb, "%s%s/%s/%s", ctx.cfg.virtual_root, reponame,
			    pagename, (filename ? filename:""));
		delim = "?";
	} else {
		strbuf_addf(&sb, "?url=%s/%s/%s", reponame, pagename,
			    (filename ? filename : ""));
		delim = "&amp;";
	}
	if (query)
		strbuf_addf(&sb, "%s%s", delim, query);
	return strbuf_detach(&sb, NULL);
}