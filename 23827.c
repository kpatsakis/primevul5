void cgit_summary_link(const char *name, const char *title, const char *class,
		       const char *head)
{
	reporevlink(NULL, name, title, class, head, NULL, NULL);
}