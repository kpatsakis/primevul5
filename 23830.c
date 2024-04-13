static int print_branch_option(const char *refname, const struct object_id *oid,
			       int flags, void *cb_data)
{
	char *name = (char *)refname;
	html_option(name, name, ctx.qry.head);
	return 0;
}