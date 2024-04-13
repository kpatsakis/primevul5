spnego_gss_inquire_cred(
			OM_uint32 *minor_status,
			gss_cred_id_t cred_handle,
			gss_name_t *name,
			OM_uint32 *lifetime,
			int *cred_usage,
			gss_OID_set *mechanisms)
{
	OM_uint32 status;
	spnego_gss_cred_id_t spcred = NULL;
	gss_cred_id_t creds = GSS_C_NO_CREDENTIAL;
	OM_uint32 tmp_minor_status;
	OM_uint32 initiator_lifetime, acceptor_lifetime;

	dsyslog("Entering inquire_cred\n");

	/*
	 * To avoid infinite recursion, if GSS_C_NO_CREDENTIAL is
	 * supplied we call gss_inquire_cred_by_mech() on the
	 * first non-SPNEGO mechanism.
	 */
	spcred = (spnego_gss_cred_id_t)cred_handle;
	if (spcred == NULL) {
		status = get_available_mechs(minor_status,
			GSS_C_NO_NAME,
			GSS_C_BOTH,
			GSS_C_NO_CRED_STORE,
			&creds,
			mechanisms);
		if (status != GSS_S_COMPLETE) {
			dsyslog("Leaving inquire_cred\n");
			return (status);
		}

		if ((*mechanisms)->count == 0) {
			gss_release_cred(&tmp_minor_status, &creds);
			gss_release_oid_set(&tmp_minor_status, mechanisms);
			dsyslog("Leaving inquire_cred\n");
			return (GSS_S_DEFECTIVE_CREDENTIAL);
		}

		assert((*mechanisms)->elements != NULL);

		status = gss_inquire_cred_by_mech(minor_status,
			creds,
			&(*mechanisms)->elements[0],
			name,
			&initiator_lifetime,
			&acceptor_lifetime,
			cred_usage);
		if (status != GSS_S_COMPLETE) {
			gss_release_cred(&tmp_minor_status, &creds);
			dsyslog("Leaving inquire_cred\n");
			return (status);
		}

		if (lifetime != NULL)
			*lifetime = (*cred_usage == GSS_C_ACCEPT) ?
				acceptor_lifetime : initiator_lifetime;

		gss_release_cred(&tmp_minor_status, &creds);
	} else {
		status = gss_inquire_cred(minor_status, spcred->mcred,
					  name, lifetime,
					  cred_usage, mechanisms);
	}

	dsyslog("Leaving inquire_cred\n");

	return (status);
}