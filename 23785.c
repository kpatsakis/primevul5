void cgit_tag_link(const char *name, const char *title, const char *class,
		   const char *tag)
{
	reporevlink("tag", name, title, class, tag, NULL, NULL);
}