const char *ldb_dn_get_rdn_name(struct ldb_dn *dn)
{
	if ( ! ldb_dn_validate(dn)) {
		return NULL;
	}
	if (dn->comp_num == 0) return NULL;
	return dn->components[0].name;
}
