void ldb_dn_extended_filter(struct ldb_dn *dn, const char * const *accept_list)
{
	unsigned int i;
	for (i=0; i<dn->ext_comp_num; i++) {
		if (!ldb_attr_in_list(accept_list, dn->ext_components[i].name)) {
			memmove(&dn->ext_components[i],
				&dn->ext_components[i+1],
				(dn->ext_comp_num-(i+1))*sizeof(dn->ext_components[0]));
			dn->ext_comp_num--;
			i--;
		}
	}
	LDB_FREE(dn->ext_linearized);
}
