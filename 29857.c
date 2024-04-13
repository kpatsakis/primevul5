int ldb_dn_compare_base(struct ldb_dn *base, struct ldb_dn *dn)
{
	int ret;
	unsigned int n_base, n_dn;

	if ( ! base || base->invalid) return 1;
	if ( ! dn || dn->invalid) return -1;

	if (( ! base->valid_case) || ( ! dn->valid_case)) {
		if (base->linearized && dn->linearized && dn->special == base->special) {
			/* try with a normal compare first, if we are lucky
			 * we will avoid exploding and casfolding */
			int dif;
			dif = strlen(dn->linearized) - strlen(base->linearized);
			if (dif < 0) {
				return dif;
			}
			if (strcmp(base->linearized,
				   &dn->linearized[dif]) == 0) {
				return 0;
			}
		}

		if ( ! ldb_dn_casefold_internal(base)) {
			return 1;
		}

		if ( ! ldb_dn_casefold_internal(dn)) {
			return -1;
		}

	}

	/* if base has more components,
	 * they don't have the same base */
	if (base->comp_num > dn->comp_num) {
		return (dn->comp_num - base->comp_num);
	}

	if ((dn->comp_num == 0) || (base->comp_num == 0)) {
		if (dn->special && base->special) {
			return strcmp(base->linearized, dn->linearized);
		} else if (dn->special) {
			return -1;
		} else if (base->special) {
			return 1;
		} else {
			return 0;
		}
	}

	n_base = base->comp_num - 1;
	n_dn = dn->comp_num - 1;

	while (n_base != (unsigned int) -1) {
		char *b_name = base->components[n_base].cf_name;
		char *dn_name = dn->components[n_dn].cf_name;

		char *b_vdata = (char *)base->components[n_base].cf_value.data;
		char *dn_vdata = (char *)dn->components[n_dn].cf_value.data;

		size_t b_vlen = base->components[n_base].cf_value.length;
		size_t dn_vlen = dn->components[n_dn].cf_value.length;

		/* compare attr names */
		ret = strcmp(b_name, dn_name);
		if (ret != 0) return ret;

		/* compare attr.cf_value. */
		if (b_vlen != dn_vlen) {
			return b_vlen - dn_vlen;
		}
		ret = strncmp(b_vdata, dn_vdata, b_vlen);
		if (ret != 0) return ret;

		n_base--;
		n_dn--;
	}

	return 0;
}
