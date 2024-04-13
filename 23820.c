void cgit_submodule_link(const char *class, char *path, const char *rev)
{
	struct string_list *list;
	struct string_list_item *item;
	char tail, *dir;
	size_t len;

	len = 0;
	tail = 0;
	list = &ctx.repo->submodules;
	item = lookup_path(list, path);
	if (!item) {
		len = strlen(path);
		tail = path[len - 1];
		if (tail == '/') {
			path[len - 1] = 0;
			item = lookup_path(list, path);
		}
	}
	if (item || ctx.repo->module_link) {
		html("<a ");
		if (class)
			htmlf("class='%s' ", class);
		html("href='");
		if (item) {
			html_attrf(item->util, rev);
		} else {
			dir = strrchr(path, '/');
			if (dir)
				dir++;
			else
				dir = path;
			html_attrf(ctx.repo->module_link, dir, rev);
		}
		html("'>");
		html_txt(path);
		html("</a>");
	} else {
		html("<span");
		if (class)
			htmlf(" class='%s'", class);
		html(">");
		html_txt(path);
		html("</span>");
	}
	html_txtf(" @ %.7s", rev);
	if (item && tail)
		path[len - 1] = tail;
}