void cgit_stats_link(const char *name, const char *title, const char *class,
		     const char *head, const char *path)
{
	reporevlink("stats", name, title, class, head, NULL, path);
}