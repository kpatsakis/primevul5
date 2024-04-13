void cgit_log_link(const char *name, const char *title, const char *class,
		   const char *head, const char *rev, const char *path,
		   int ofs, const char *grep, const char *pattern, int showmsg,
		   int follow)
{
	char *delim;

	delim = repolink(title, class, "log", head, path);
	if (rev && ctx.qry.head && strcmp(rev, ctx.qry.head)) {
		html(delim);
		html("id=");
		html_url_arg(rev);
		delim = "&amp;";
	}
	if (grep && pattern) {
		html(delim);
		html("qt=");
		html_url_arg(grep);
		delim = "&amp;";
		html(delim);
		html("q=");
		html_url_arg(pattern);
	}
	if (ofs > 0) {
		html(delim);
		html("ofs=");
		htmlf("%d", ofs);
		delim = "&amp;";
	}
	if (showmsg) {
		html(delim);
		html("showmsg=1");
		delim = "&amp;";
	}
	if (follow) {
		html(delim);
		html("follow=1");
	}
	html("'>");
	html_txt(name);
	html("</a>");
}