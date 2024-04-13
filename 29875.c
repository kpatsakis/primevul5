bool ldb_dn_is_null(struct ldb_dn *dn)
{
	if ( ! dn || dn->invalid) return false;
	if (ldb_dn_has_extended(dn)) return false;
	if (dn->linearized && (dn->linearized[0] == '\0')) return true;
	return false;
}
