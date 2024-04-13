static int check_ref(const char *name, unsigned int flags)
{
	if (!flags)
		return 1;

	if (!skip_prefix(name, "refs/", &name))
		return 0;

	/* REF_NORMAL means that we don't want the magic fake tag refs */
	if ((flags & REF_NORMAL) && check_refname_format(name, 0))
		return 0;

	/* REF_HEADS means that we want regular branch heads */
	if ((flags & REF_HEADS) && starts_with(name, "heads/"))
		return 1;

	/* REF_TAGS means that we want tags */
	if ((flags & REF_TAGS) && starts_with(name, "tags/"))
		return 1;

	/* All type bits clear means that we are ok with anything */
	return !(flags & ~REF_NORMAL);
}