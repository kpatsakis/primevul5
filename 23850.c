void cgit_patch_link(const char *name, const char *title, const char *class,
		     const char *head, const char *rev, const char *path)
{
	reporevlink("patch", name, title, class, head, rev, path);
}