bool ldb_dn_is_special(struct ldb_dn *dn)
{
	if ( ! dn || dn->invalid) return false;
	return dn->special;
}
