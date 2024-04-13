struct ldb_dn *ldb_dn_from_ldb_val(TALLOC_CTX *mem_ctx,
                                   struct ldb_context *ldb,
                                   const struct ldb_val *strdn)
{
	struct ldb_dn *dn;

	if (! ldb) return NULL;

	if (strdn && strdn->data
	    && (strnlen((const char*)strdn->data, strdn->length) != strdn->length)) {
		/* The RDN must not contain a character with value 0x0 */
		return NULL;
	}

	dn = talloc_zero(mem_ctx, struct ldb_dn);
	LDB_DN_NULL_FAILED(dn);

	dn->ldb = talloc_get_type(ldb, struct ldb_context);
	if (dn->ldb == NULL) {
		/* the caller probably got the arguments to
		   ldb_dn_new() mixed up */
		talloc_free(dn);
		return NULL;
	}

	if (strdn->data && strdn->length) {
		const char *data = (const char *)strdn->data;
		size_t length = strdn->length;

		if (data[0] == '@') {
			dn->special = true;
		}
		dn->ext_linearized = talloc_strndup(dn, data, length);
		LDB_DN_NULL_FAILED(dn->ext_linearized);

		if (data[0] == '<') {
			const char *p_save, *p = dn->ext_linearized;
			do {
				p_save = p;
				p = strstr(p, ">;");
				if (p) {
					p = p + 2;
				}
			} while (p);

			if (p_save == dn->ext_linearized) {
				dn->linearized = talloc_strdup(dn, "");
			} else {
				dn->linearized = talloc_strdup(dn, p_save);
			}
			LDB_DN_NULL_FAILED(dn->linearized);
		} else {
			dn->linearized = dn->ext_linearized;
			dn->ext_linearized = NULL;
		}
	} else {
		dn->linearized = talloc_strdup(dn, "");
		LDB_DN_NULL_FAILED(dn->linearized);
	}

	return dn;

failed:
	talloc_free(dn);
	return NULL;
}
