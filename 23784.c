void cgit_diff_link(const char *name, const char *title, const char *class,
		    const char *head, const char *new_rev, const char *old_rev,
		    const char *path)
{
	char *delim;

	delim = repolink(title, class, "diff", head, path);
	if (new_rev && ctx.qry.head != NULL && strcmp(new_rev, ctx.qry.head)) {
		html(delim);
		html("id=");
		html_url_arg(new_rev);
		delim = "&amp;";
	}
	if (old_rev) {
		html(delim);
		html("id2=");
		html_url_arg(old_rev);
		delim = "&amp;";
	}
	if (ctx.qry.difftype) {
		html(delim);
		htmlf("dt=%d", ctx.qry.difftype);
		delim = "&amp;";
	}
	if (ctx.qry.context > 0 && ctx.qry.context != 3) {
		html(delim);
		html("context=");
		htmlf("%d", ctx.qry.context);
		delim = "&amp;";
	}
	if (ctx.qry.ignorews) {
		html(delim);
		html("ignorews=1");
		delim = "&amp;";
	}
	if (ctx.qry.follow) {
		html(delim);
		html("follow=1");
	}
	html("'>");
	html_txt(name);
	html("</a>");
}