spnego_gss_inquire_names_for_mech(
				OM_uint32	*minor_status,
				gss_OID		mechanism,
				gss_OID_set	*name_types)
{
	OM_uint32   major, minor;

	dsyslog("Entering inquire_names_for_mech\n");
	/*
	 * We only know how to handle our own mechanism.
	 */
	if ((mechanism != GSS_C_NULL_OID) &&
	    !g_OID_equal(gss_mech_spnego, mechanism)) {
		*minor_status = 0;
		return (GSS_S_FAILURE);
	}

	major = gss_create_empty_oid_set(minor_status, name_types);
	if (major == GSS_S_COMPLETE) {
		/* Now add our members. */
		if (((major = gss_add_oid_set_member(minor_status,
				(gss_OID) GSS_C_NT_USER_NAME,
				name_types)) == GSS_S_COMPLETE) &&
		    ((major = gss_add_oid_set_member(minor_status,
				(gss_OID) GSS_C_NT_MACHINE_UID_NAME,
				name_types)) == GSS_S_COMPLETE) &&
		    ((major = gss_add_oid_set_member(minor_status,
				(gss_OID) GSS_C_NT_STRING_UID_NAME,
				name_types)) == GSS_S_COMPLETE)) {
			major = gss_add_oid_set_member(minor_status,
				(gss_OID) GSS_C_NT_HOSTBASED_SERVICE,
				name_types);
		}

		if (major != GSS_S_COMPLETE)
			(void) gss_release_oid_set(&minor, name_types);
	}

	dsyslog("Leaving inquire_names_for_mech\n");
	return (major);
}