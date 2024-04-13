bool ldb_dn_add_child(struct ldb_dn *dn, struct ldb_dn *child)
{
	const char *s;
	char *t;

	if ( !child || child->invalid || !dn || dn->invalid) {
		return false;
	}

	if (dn->components) {
		unsigned int n;
		unsigned int i, j;

		if (dn->comp_num == 0) {
			return false;
		}

		if ( ! ldb_dn_validate(child)) {
			return false;
		}

		s = NULL;
		if (dn->valid_case) {
			if ( ! (s = ldb_dn_get_casefold(child))) {
				return false;
			}
		}

		n = dn->comp_num + child->comp_num;

		dn->components = talloc_realloc(dn,
						dn->components,
						struct ldb_dn_component,
						n);
		if ( ! dn->components) {
			ldb_dn_mark_invalid(dn);
			return false;
		}

		for (i = dn->comp_num - 1, j = n - 1; i != (unsigned int) -1;
		     i--, j--) {
			dn->components[j] = dn->components[i];
		}

		for (i = 0; i < child->comp_num; i++) {
			dn->components[i] =
				ldb_dn_copy_component(dn->components,
							&child->components[i]);
			if (dn->components[i].value.data == NULL) {
				ldb_dn_mark_invalid(dn);
				return false;
			}
		}

		dn->comp_num = n;

		if (dn->casefold && s) {
			t = talloc_asprintf(dn, "%s,%s", s, dn->casefold);
			LDB_FREE(dn->casefold);
			dn->casefold = t;
		}
	}

	if (dn->linearized) {
		if (dn->linearized[0] == '\0') {
			return false;
		}

		s = ldb_dn_get_linearized(child);
		if ( ! s) {
			return false;
		}

		t = talloc_asprintf(dn, "%s,%s", s, dn->linearized);
		if ( ! t) {
			ldb_dn_mark_invalid(dn);
			return false;
		}
		LDB_FREE(dn->linearized);
		dn->linearized = t;
	}

	/* Wipe the ext_linearized DN,
	 * the GUID and SID are almost certainly no longer valid */
	LDB_FREE(dn->ext_linearized);
	LDB_FREE(dn->ext_components);
	dn->ext_comp_num = 0;

	return true;
}
