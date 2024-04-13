void cgit_commit_link(char *name, const char *title, const char *class,
		      const char *head, const char *rev, const char *path)
{
	if (strlen(name) > ctx.cfg.max_msg_len && ctx.cfg.max_msg_len >= 15) {
		name[ctx.cfg.max_msg_len] = '\0';
		name[ctx.cfg.max_msg_len - 1] = '.';
		name[ctx.cfg.max_msg_len - 2] = '.';
		name[ctx.cfg.max_msg_len - 3] = '.';
	}

	char *delim;

	delim = repolink(title, class, "commit", head, path);
	if (rev && ctx.qry.head && strcmp(rev, ctx.qry.head)) {
		html(delim);
		html("id=");
		html_url_arg(rev);
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
	if (name[0] != '\0')
		html_txt(name);
	else
		html_txt("(no commit message)");
	html("</a>");
}