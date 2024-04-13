const struct ldb_val *ldb_dn_get_rdn_val(struct ldb_dn *dn)
{
	if ( ! ldb_dn_validate(dn)) {
		return NULL;
	}
	if (dn->comp_num == 0) return NULL;
	return &dn->components[0].value;
}
