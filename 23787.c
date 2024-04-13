static void cgit_print_path_crumbs(char *path)
{
	char *old_path = ctx.qry.path;
	char *p = path, *q, *end = path + strlen(path);

	ctx.qry.path = NULL;
	cgit_self_link("root", NULL, NULL);
	ctx.qry.path = p = path;
	while (p < end) {
		if (!(q = strchr(p, '/')))
			q = end;
		*q = '\0';
		html_txt("/");
		cgit_self_link(p, NULL, NULL);
		if (q < end)
			*q = '/';
		p = q + 1;
	}
	ctx.qry.path = old_path;
}