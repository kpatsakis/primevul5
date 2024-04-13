static void parse_one_symref_info(struct string_list *symref, const char *val, int len)
{
	char *sym, *target;
	struct string_list_item *item;

	if (!len)
		return; /* just "symref" */
	/* e.g. "symref=HEAD:refs/heads/master" */
	sym = xmemdupz(val, len);
	target = strchr(sym, ':');
	if (!target)
		/* just "symref=something" */
		goto reject;
	*(target++) = '\0';
	if (check_refname_format(sym, REFNAME_ALLOW_ONELEVEL) ||
	    check_refname_format(target, REFNAME_ALLOW_ONELEVEL))
		/* "symref=bogus:pair */
		goto reject;
	item = string_list_append_nodup(symref, sym);
	item->util = target;
	return;
reject:
	free(sym);
	return;
}