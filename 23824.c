static void print_rel_vcs_link(const char *url)
{
	html("<link rel='vcs-git' href='");
	html_attr(url);
	html("' title='");
	html_attr(ctx.repo->name);
	html(" Git repository'/>\n");
}