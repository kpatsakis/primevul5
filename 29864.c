int ldb_dn_get_comp_num(struct ldb_dn *dn)
{
	if ( ! ldb_dn_validate(dn)) {
		return -1;
	}
	return dn->comp_num;
}
