void cgit_index_link(const char *name, const char *title, const char *class,
		     const char *pattern, const char *sort, int ofs, int always_root)
{
	site_link(NULL, name, title, class, pattern, sort, ofs, always_root);
}