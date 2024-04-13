bool ldb_dn_check_special(struct ldb_dn *dn, const char *check)
{
	if ( ! dn || dn->invalid) return false;
	return ! strcmp(dn->linearized, check);
}
