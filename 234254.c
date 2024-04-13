static int print_ref_info(const char *refname, const unsigned char *sha1,
                          int flags, void *cb_data)
{
	struct object *obj;

	if (!(obj = parse_object(sha1)))
		return 0;

	if (!strcmp(refname, "HEAD") || !prefixcmp(refname, "refs/heads/"))
		htmlf("%s\t%s\n", sha1_to_hex(sha1), refname);
	else if (!prefixcmp(refname, "refs/tags") && obj->type == OBJ_TAG) {
		if (!(obj = deref_tag(obj, refname, 0)))
			return 0;
		htmlf("%s\t%s\n", sha1_to_hex(sha1), refname);
		htmlf("%s\t%s^{}\n", sha1_to_hex(obj->sha1), refname);
	}
	return 0;
}