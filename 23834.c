void cgit_refs_link(const char *name, const char *title, const char *class,
		    const char *head, const char *rev, const char *path)
{
	reporevlink("refs", name, title, class, head, rev, path);
}