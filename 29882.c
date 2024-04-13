int ldb_dn_set_component(struct ldb_dn *dn, int num,
			 const char *name, const struct ldb_val val)
{
	char *n;
	struct ldb_val v;

	if ( ! ldb_dn_validate(dn)) {
		return LDB_ERR_OTHER;
	}

	if (num >= dn->comp_num) {
		return LDB_ERR_OTHER;
	}

	n = talloc_strdup(dn, name);
	if ( ! n) {
		return LDB_ERR_OTHER;
	}

	v.length = val.length;
	v.data = (uint8_t *)talloc_memdup(dn, val.data, v.length+1);
	if ( ! v.data) {
		talloc_free(n);
		return LDB_ERR_OTHER;
	}

	talloc_free(dn->components[num].name);
	talloc_free(dn->components[num].value.data);
	dn->components[num].name = n;
	dn->components[num].value = v;

	if (dn->valid_case) {
		unsigned int i;
		for (i = 0; i < dn->comp_num; i++) {
			LDB_FREE(dn->components[i].cf_name);
			LDB_FREE(dn->components[i].cf_value.data);
		}
		dn->valid_case = false;
	}
	LDB_FREE(dn->casefold);
	LDB_FREE(dn->linearized);

	/* Wipe the ext_linearized DN,
	 * the GUID and SID are almost certainly no longer valid */
	LDB_FREE(dn->ext_linearized);
	LDB_FREE(dn->ext_components);
	dn->ext_comp_num = 0;

	return LDB_SUCCESS;
}
