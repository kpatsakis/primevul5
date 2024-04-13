static void reporevlink(const char *page, const char *name, const char *title,
			const char *class, const char *head, const char *rev,
			const char *path)
{
	char *delim;

	delim = repolink(title, class, page, head, path);
	if (rev && ctx.qry.head != NULL && strcmp(rev, ctx.qry.head)) {
		html(delim);
		html("id=");
		html_url_arg(rev);
	}
	html("'>");
	html_txt(name);
	html("</a>");
}