void cgit_redirect(const char *url, bool permanent)
{
	htmlf("Status: %d %s\n", permanent ? 301 : 302, permanent ? "Moved" : "Found");
	html("Location: ");
	html_url_path(url);
	html("\n\n");
	exit(0);
}