struct ldb_context *ldb_dn_get_ldb_context(struct ldb_dn *dn)
{
	return dn->ldb;
}
