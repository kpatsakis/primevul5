bool ldb_dn_add_base_fmt(struct ldb_dn *dn, const char *base_fmt, ...)
{
	struct ldb_dn *base;
	char *base_str;
	va_list ap;
	bool ret;

	if ( !dn || dn->invalid) {
		return false;
	}

	va_start(ap, base_fmt);
	base_str = talloc_vasprintf(dn, base_fmt, ap);
	va_end(ap);

	if (base_str == NULL) {
		return false;
	}

	base = ldb_dn_new(base_str, dn->ldb, base_str);

	ret = ldb_dn_add_base(dn, base);

	talloc_free(base_str);

	return ret;
}
