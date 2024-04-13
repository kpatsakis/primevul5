negotiate_mech(gss_OID_set supported, gss_OID_set received,
	       OM_uint32 *negResult)
{
	size_t i, j;

	for (i = 0; i < received->count; i++) {
		gss_OID mech_oid = &received->elements[i];

		/* Accept wrong mechanism OID from MS clients */
		if (g_OID_equal(mech_oid, &gss_mech_krb5_wrong_oid))
			mech_oid = (gss_OID)&gss_mech_krb5_oid;

		for (j = 0; j < supported->count; j++) {
			if (g_OID_equal(mech_oid, &supported->elements[j])) {
				*negResult = (i == 0) ? ACCEPT_INCOMPLETE :
					REQUEST_MIC;
				return &received->elements[i];
			}
		}
	}
	*negResult = REJECT;
	return (NULL);
}