static bool ldb_dn_casefold_internal(struct ldb_dn *dn)
{
	unsigned int i;
	int ret;

	if ( ! dn || dn->invalid) return false;

	if (dn->valid_case) return true;

	if (( ! dn->components) && ( ! ldb_dn_explode(dn))) {
		return false;
	}

	for (i = 0; i < dn->comp_num; i++) {
		const struct ldb_schema_attribute *a;

		dn->components[i].cf_name =
			ldb_attr_casefold(dn->components,
					  dn->components[i].name);
		if (!dn->components[i].cf_name) {
			goto failed;
		}

		a = ldb_schema_attribute_by_name(dn->ldb,
						 dn->components[i].cf_name);

		ret = a->syntax->canonicalise_fn(dn->ldb, dn->components,
						 &(dn->components[i].value),
						 &(dn->components[i].cf_value));
		if (ret != 0) {
			goto failed;
		}
	}

	dn->valid_case = true;

	return true;

failed:
	for (i = 0; i < dn->comp_num; i++) {
		LDB_FREE(dn->components[i].cf_name);
		LDB_FREE(dn->components[i].cf_value.data);
	}
	return false;
}
