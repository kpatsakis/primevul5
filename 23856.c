void cgit_plain_link(const char *name, const char *title, const char *class,
		     const char *head, const char *rev, const char *path)
{
	reporevlink("plain", name, title, class, head, rev, path);
}