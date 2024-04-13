const struct ldb_val *ldb_dn_get_extended_component(struct ldb_dn *dn,
						    const char *name)
{
	unsigned int i;
	if ( ! ldb_dn_validate(dn)) {
		return NULL;
	}
	for (i=0; i < dn->ext_comp_num; i++) {
		if (ldb_attr_cmp(dn->ext_components[i].name, name) == 0) {
			return &dn->ext_components[i].value;
		}
	}
	return NULL;
}
