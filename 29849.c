bool ldb_dn_add_base(struct ldb_dn *dn, struct ldb_dn *base)
{
	const char *s;
	char *t;

	if ( !base || base->invalid || !dn || dn->invalid) {
		return false;
	}

	if (dn->components) {
		unsigned int i;

		if ( ! ldb_dn_validate(base)) {
			return false;
		}

		s = NULL;
		if (dn->valid_case) {
			if ( ! (s = ldb_dn_get_casefold(base))) {
				return false;
			}
		}

		dn->components = talloc_realloc(dn,
						dn->components,
						struct ldb_dn_component,
						dn->comp_num + base->comp_num);
		if ( ! dn->components) {
			ldb_dn_mark_invalid(dn);
			return false;
		}

		for (i = 0; i < base->comp_num; dn->comp_num++, i++) {
			dn->components[dn->comp_num] =
				ldb_dn_copy_component(dn->components,
							&base->components[i]);
			if (dn->components[dn->comp_num].value.data == NULL) {
				ldb_dn_mark_invalid(dn);
				return false;
			}
		}

		if (dn->casefold && s) {
			if (*dn->casefold) {
				t = talloc_asprintf(dn, "%s,%s",
						    dn->casefold, s);
			} else {
				t = talloc_strdup(dn, s);
			}
			LDB_FREE(dn->casefold);
			dn->casefold = t;
		}
	}

	if (dn->linearized) {

		s = ldb_dn_get_linearized(base);
		if ( ! s) {
			return false;
		}

		if (*dn->linearized) {
			t = talloc_asprintf(dn, "%s,%s",
					    dn->linearized, s);
		} else {
			t = talloc_strdup(dn, s);
		}
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
