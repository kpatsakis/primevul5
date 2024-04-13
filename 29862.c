static int ldb_dn_extended_component_compare(const void *p1, const void *p2)
{
	const struct ldb_dn_ext_component *ec1 = (const struct ldb_dn_ext_component *)p1;
	const struct ldb_dn_ext_component *ec2 = (const struct ldb_dn_ext_component *)p2;
	return strcmp(ec1->name, ec2->name);
}
