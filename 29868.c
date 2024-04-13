char *ldb_dn_get_extended_linearized(TALLOC_CTX *mem_ctx, struct ldb_dn *dn, int mode)
{
	const char *linearized = ldb_dn_get_linearized(dn);
	char *p = NULL;
	unsigned int i;

	if (!linearized) {
		return NULL;
	}

	if (!ldb_dn_has_extended(dn)) {
		return talloc_strdup(mem_ctx, linearized);
	}

	if (!ldb_dn_validate(dn)) {
		return NULL;
	}

	/* sort the extended components by name. The idea is to make
	 * the resulting DNs consistent, plus to ensure that we put
	 * 'DELETED' first, so it can be very quickly recognised
	 */
	TYPESAFE_QSORT(dn->ext_components, dn->ext_comp_num,
		       ldb_dn_extended_component_compare);

	for (i = 0; i < dn->ext_comp_num; i++) {
		const struct ldb_dn_extended_syntax *ext_syntax;
		const char *name = dn->ext_components[i].name;
		struct ldb_val ec_val = dn->ext_components[i].value;
		struct ldb_val val;
		int ret;

		ext_syntax = ldb_dn_extended_syntax_by_name(dn->ldb, name);
		if (!ext_syntax) {
			return NULL;
		}

		if (mode == 1) {
			ret = ext_syntax->write_clear_fn(dn->ldb, mem_ctx,
							&ec_val, &val);
		} else if (mode == 0) {
			ret = ext_syntax->write_hex_fn(dn->ldb, mem_ctx,
							&ec_val, &val);
		} else {
			ret = -1;
		}

		if (ret != LDB_SUCCESS) {
			return NULL;
		}

		if (i == 0) {
			p = talloc_asprintf(mem_ctx, "<%s=%s>", 
					    name, val.data);
		} else {
			p = talloc_asprintf_append_buffer(p, ";<%s=%s>",
							  name, val.data);
		}

		talloc_free(val.data);

		if (!p) {
			return NULL;
		}
	}

	if (dn->ext_comp_num && *linearized) {
		p = talloc_asprintf_append_buffer(p, ";%s", linearized);
	}

	if (!p) {
		return NULL;
	}

	return p;
}
