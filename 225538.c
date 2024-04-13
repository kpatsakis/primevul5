spnego_gss_inquire_attrs_for_mech(OM_uint32 *minor_status,
				  gss_const_OID mech,
				  gss_OID_set *mech_attrs,
				  gss_OID_set *known_mech_attrs)
{
	OM_uint32 major, tmpMinor;

	/* known_mech_attrs is handled by mechglue */
	*minor_status = 0;

	if (mech_attrs == NULL)
	    return (GSS_S_COMPLETE);

	major = gss_create_empty_oid_set(minor_status, mech_attrs);
	if (GSS_ERROR(major))
		goto cleanup;

#define MA_SUPPORTED(ma)    do {					\
		major = gss_add_oid_set_member(minor_status,		\
					       (gss_OID)ma, mech_attrs); \
		if (GSS_ERROR(major))					\
			goto cleanup;					\
	} while (0)

	MA_SUPPORTED(GSS_C_MA_MECH_NEGO);
	MA_SUPPORTED(GSS_C_MA_ITOK_FRAMED);

cleanup:
	if (GSS_ERROR(major))
		gss_release_oid_set(&tmpMinor, mech_attrs);

	return (major);
}