bool ldb_dn_add_child_fmt(struct ldb_dn *dn, const char *child_fmt, ...)
{
	struct ldb_dn *child;
	char *child_str;
	va_list ap;
	bool ret;

	if ( !dn || dn->invalid) {
		return false;
	}

	va_start(ap, child_fmt);
	child_str = talloc_vasprintf(dn, child_fmt, ap);
	va_end(ap);

	if (child_str == NULL) {
		return false;
	}

	child = ldb_dn_new(child_str, dn->ldb, child_str);

	ret = ldb_dn_add_child(dn, child);

	talloc_free(child_str);

	return ret;
}
