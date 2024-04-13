struct ldb_dn *ldb_dn_new_fmt(TALLOC_CTX *mem_ctx,
			      struct ldb_context *ldb,
			      const char *new_fmt, ...)
{
	char *strdn;
	va_list ap;

	if ( (! mem_ctx) || (! ldb)) return NULL;

	va_start(ap, new_fmt);
	strdn = talloc_vasprintf(mem_ctx, new_fmt, ap);
	va_end(ap);

	if (strdn) {
		struct ldb_dn *dn = ldb_dn_new(mem_ctx, ldb, strdn);
		talloc_free(strdn);
		return dn;
	}

	return NULL;
}
