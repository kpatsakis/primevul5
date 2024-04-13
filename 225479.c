is_kerb_mech(gss_OID oid)
{
	int answer = 0;
	OM_uint32 minor;
	extern const gss_OID_set_desc * const gss_mech_set_krb5_both;

	(void) gss_test_oid_set_member(&minor,
		oid, (gss_OID_set)gss_mech_set_krb5_both, &answer);

	return (answer);
}