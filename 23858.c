static char *repolink(const char *title, const char *class, const char *page,
		      const char *head, const char *path)
{
	char *delim = "?";

	html("<a");
	if (title) {
		html(" title='");
		html_attr(title);
		html("'");
	}
	if (class) {
		html(" class='");
		html_attr(class);
		html("'");
	}
	html(" href='");
	if (ctx.cfg.virtual_root) {
		html_url_path(ctx.cfg.virtual_root);
		html_url_path(ctx.repo->url);
		if (ctx.repo->url[strlen(ctx.repo->url) - 1] != '/')
			html("/");
		if (page) {
			html_url_path(page);
			html("/");
			if (path)
				html_url_path(path);
		}
	} else {
		html_url_path(ctx.cfg.script_name);
		html("?url=");
		html_url_arg(ctx.repo->url);
		if (ctx.repo->url[strlen(ctx.repo->url) - 1] != '/')
			html("/");
		if (page) {
			html_url_arg(page);
			html("/");
			if (path)
				html_url_arg(path);
		}
		delim = "&amp;";
	}
	if (head && strcmp(head, ctx.repo->defbranch)) {
		html(delim);
		html("h=");
		html_url_arg(head);
		delim = "&amp;";
	}
	return fmt("%s", delim);
}