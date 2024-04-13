const struct ldb_val *ldb_dn_get_component_val(struct ldb_dn *dn,
						unsigned int num)
{
	if ( ! ldb_dn_validate(dn)) {
		return NULL;
	}
	if (num >= dn->comp_num) return NULL;
	return &dn->components[num].value;
}
