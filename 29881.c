bool ldb_dn_replace_components(struct ldb_dn *dn, struct ldb_dn *new_dn)
{
	int i;

	if ( ! ldb_dn_validate(dn) || ! ldb_dn_validate(new_dn)) {
		return false;
	}

	/* free components */
	for (i = 0; i < dn->comp_num; i++) {
		LDB_FREE(dn->components[i].name);
		LDB_FREE(dn->components[i].value.data);
		LDB_FREE(dn->components[i].cf_name);
		LDB_FREE(dn->components[i].cf_value.data);
	}

	dn->components = talloc_realloc(dn,
					dn->components,
					struct ldb_dn_component,
					new_dn->comp_num);
	if (dn->components == NULL) {
		ldb_dn_mark_invalid(dn);
		return false;
	}

	dn->comp_num = new_dn->comp_num;
	dn->valid_case = new_dn->valid_case;

	for (i = 0; i < dn->comp_num; i++) {
		dn->components[i] = ldb_dn_copy_component(dn->components, &new_dn->components[i]);
		if (dn->components[i].name == NULL) {
			ldb_dn_mark_invalid(dn);
			return false;
		}
	}
	if (new_dn->linearized == NULL) {
		dn->linearized = NULL;
	} else {
		dn->linearized = talloc_strdup(dn, new_dn->linearized);
		if (dn->linearized == NULL) {
			ldb_dn_mark_invalid(dn);
			return false;
		}
	}

	return true;
}
