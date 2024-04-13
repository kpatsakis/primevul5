bool ldb_dn_is_valid(struct ldb_dn *dn)
{
	if ( ! dn) return false;
	return ! dn->invalid;
}
