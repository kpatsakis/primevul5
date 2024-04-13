void cgit_tree_link(const char *name, const char *title, const char *class,
		    const char *head, const char *rev, const char *path)
{
	reporevlink("tree", name, title, class, head, rev, path);
}