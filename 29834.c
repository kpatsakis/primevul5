static void ldb_dn_mark_invalid(struct ldb_dn *dn)
{
	dn->invalid = true;
}
