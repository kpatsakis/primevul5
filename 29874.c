bool ldb_dn_has_extended(struct ldb_dn *dn)
{
	if ( ! dn || dn->invalid) return false;
	if (dn->ext_linearized && (dn->ext_linearized[0] == '<')) return true;
	return dn->ext_comp_num != 0;
}
