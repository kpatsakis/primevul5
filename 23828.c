static void site_url(const char *page, const char *search, const char *sort, int ofs, int always_root)
{
	char *delim = "?";

	if (always_root || page)
		html_attr(cgit_rooturl());
	else {
		char *currenturl = cgit_currenturl();
		html_attr(currenturl);
		free(currenturl);
	}

	if (page) {
		htmlf("?p=%s", page);
		delim = "&amp;";
	}
	if (search) {
		html(delim);
		html("q=");
		html_attr(search);
		delim = "&amp;";
	}
	if (sort) {
		html(delim);
		html("s=");
		html_attr(sort);
		delim = "&amp;";
	}
	if (ofs) {
		html(delim);
		htmlf("ofs=%d", ofs);
	}
}