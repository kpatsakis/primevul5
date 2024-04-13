void ldb_dn_remove_extended_components(struct ldb_dn *dn)
{
	LDB_FREE(dn->ext_linearized);
	LDB_FREE(dn->ext_components);
	dn->ext_comp_num = 0;
}
