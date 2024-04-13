int ldb_dn_set_extended_component(struct ldb_dn *dn,
				  const char *name, const struct ldb_val *val)
{
	struct ldb_dn_ext_component *p;
	unsigned int i;
	struct ldb_val v2;

	if ( ! ldb_dn_validate(dn)) {
		return LDB_ERR_OTHER;
	}

	if (!ldb_dn_extended_syntax_by_name(dn->ldb, name)) {
		/* We don't know how to handle this type of thing */
		return LDB_ERR_INVALID_DN_SYNTAX;
	}

	for (i=0; i < dn->ext_comp_num; i++) {
		if (ldb_attr_cmp(dn->ext_components[i].name, name) == 0) {
			if (val) {
				dn->ext_components[i].value =
					ldb_val_dup(dn->ext_components, val);

				dn->ext_components[i].name =
					talloc_strdup(dn->ext_components, name);
				if (!dn->ext_components[i].name ||
				    !dn->ext_components[i].value.data) {
					ldb_dn_mark_invalid(dn);
					return LDB_ERR_OPERATIONS_ERROR;
				}
			} else {
				if (i != (dn->ext_comp_num - 1)) {
					memmove(&dn->ext_components[i],
						&dn->ext_components[i+1],
						((dn->ext_comp_num-1) - i) *
						  sizeof(*dn->ext_components));
				}
				dn->ext_comp_num--;

				dn->ext_components = talloc_realloc(dn,
						   dn->ext_components,
						   struct ldb_dn_ext_component,
						   dn->ext_comp_num);
				if (!dn->ext_components) {
					ldb_dn_mark_invalid(dn);
					return LDB_ERR_OPERATIONS_ERROR;
				}
			}
			LDB_FREE(dn->ext_linearized);

			return LDB_SUCCESS;
		}
	}

	if (val == NULL) {
		/* removing a value that doesn't exist is not an error */
		return LDB_SUCCESS;
	}

	v2 = *val;

	p = dn->ext_components
		= talloc_realloc(dn,
				 dn->ext_components,
				 struct ldb_dn_ext_component,
				 dn->ext_comp_num + 1);
	if (!dn->ext_components) {
		ldb_dn_mark_invalid(dn);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	p[dn->ext_comp_num].value = ldb_val_dup(dn->ext_components, &v2);
	p[dn->ext_comp_num].name = talloc_strdup(p, name);

	if (!dn->ext_components[i].name || !dn->ext_components[i].value.data) {
		ldb_dn_mark_invalid(dn);
		return LDB_ERR_OPERATIONS_ERROR;
	}
	dn->ext_components = p;
	dn->ext_comp_num++;

	LDB_FREE(dn->ext_linearized);

	return LDB_SUCCESS;
}
