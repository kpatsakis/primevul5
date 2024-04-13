bool ldb_dn_remove_base_components(struct ldb_dn *dn, unsigned int num)
{
	unsigned int i;

	if ( ! ldb_dn_validate(dn)) {
		return false;
	}

	if (dn->comp_num < num) {
		return false;
	}

	/* free components */
	for (i = dn->comp_num - num; i < dn->comp_num; i++) {
		LDB_FREE(dn->components[i].name);
		LDB_FREE(dn->components[i].value.data);
		LDB_FREE(dn->components[i].cf_name);
		LDB_FREE(dn->components[i].cf_value.data);
	}

	dn->comp_num -= num;

	if (dn->valid_case) {
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

	return true;
}
