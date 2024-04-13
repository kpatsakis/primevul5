static void site_link(const char *page, const char *name, const char *title,
		      const char *class, const char *search, const char *sort, int ofs, int always_root)
{
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
	site_url(page, search, sort, ofs, always_root);
	html("'>");
	html_txt(name);
	html("</a>");
}