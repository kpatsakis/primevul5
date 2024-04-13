struct ldb_dn *ldb_dn_copy(TALLOC_CTX *mem_ctx, struct ldb_dn *dn)
{
	struct ldb_dn *new_dn;

	if (!dn || dn->invalid) {
		return NULL;
	}

	new_dn = talloc_zero(mem_ctx, struct ldb_dn);
	if ( !new_dn) {
		return NULL;
	}

	*new_dn = *dn;

	if (dn->components) {
		unsigned int i;

		new_dn->components =
			talloc_zero_array(new_dn,
					  struct ldb_dn_component,
					  dn->comp_num);
		if ( ! new_dn->components) {
			talloc_free(new_dn);
			return NULL;
		}

		for (i = 0; i < dn->comp_num; i++) {
			new_dn->components[i] =
				ldb_dn_copy_component(new_dn->components,
						      &dn->components[i]);
			if ( ! new_dn->components[i].value.data) {
				talloc_free(new_dn);
				return NULL;
			}
		}
	}

	if (dn->ext_components) {
		unsigned int i;

		new_dn->ext_components =
			talloc_zero_array(new_dn,
					  struct ldb_dn_ext_component,
					  dn->ext_comp_num);
		if ( ! new_dn->ext_components) {
			talloc_free(new_dn);
			return NULL;
		}

		for (i = 0; i < dn->ext_comp_num; i++) {
			new_dn->ext_components[i] =
				 ldb_dn_ext_copy_component(
						new_dn->ext_components,
						&dn->ext_components[i]);
			if ( ! new_dn->ext_components[i].value.data) {
				talloc_free(new_dn);
				return NULL;
			}
		}
	}

	if (dn->casefold) {
		new_dn->casefold = talloc_strdup(new_dn, dn->casefold);
		if ( ! new_dn->casefold) {
			talloc_free(new_dn);
			return NULL;
		}
	}

	if (dn->linearized) {
		new_dn->linearized = talloc_strdup(new_dn, dn->linearized);
		if ( ! new_dn->linearized) {
			talloc_free(new_dn);
			return NULL;
		}
	}

	if (dn->ext_linearized) {
		new_dn->ext_linearized = talloc_strdup(new_dn,
							dn->ext_linearized);
		if ( ! new_dn->ext_linearized) {
			talloc_free(new_dn);
			return NULL;
		}
	}

	return new_dn;
}
