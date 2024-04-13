spnego_gss_acquire_cred_impersonate_name(OM_uint32 *minor_status,
					 const gss_cred_id_t impersonator_cred_handle,
					 const gss_name_t desired_name,
					 OM_uint32 time_req,
					 gss_OID_set desired_mechs,
					 gss_cred_usage_t cred_usage,
					 gss_cred_id_t *output_cred_handle,
					 gss_OID_set *actual_mechs,
					 OM_uint32 *time_rec)
{
	OM_uint32 status;
	gss_OID_set amechs = GSS_C_NULL_OID_SET;
	spnego_gss_cred_id_t imp_spcred = NULL, out_spcred = NULL;
	gss_cred_id_t imp_mcred, out_mcred;

	dsyslog("Entering spnego_gss_acquire_cred_impersonate_name\n");

	if (actual_mechs)
		*actual_mechs = NULL;

	if (time_rec)
		*time_rec = 0;

	imp_spcred = (spnego_gss_cred_id_t)impersonator_cred_handle;
	imp_mcred = imp_spcred ? imp_spcred->mcred : GSS_C_NO_CREDENTIAL;
	if (desired_mechs == GSS_C_NO_OID_SET) {
		status = gss_inquire_cred(minor_status, imp_mcred, NULL, NULL,
					  NULL, &amechs);
		if (status != GSS_S_COMPLETE)
			return status;

		desired_mechs = amechs;
	}

	status = gss_acquire_cred_impersonate_name(minor_status, imp_mcred,
						   desired_name, time_req,
						   desired_mechs, cred_usage,
						   &out_mcred, actual_mechs,
						   time_rec);

	if (amechs != GSS_C_NULL_OID_SET)
		(void) gss_release_oid_set(minor_status, &amechs);

	out_spcred = malloc(sizeof(spnego_gss_cred_id_rec));
	if (out_spcred == NULL) {
		gss_release_cred(minor_status, &out_mcred);
		*minor_status = ENOMEM;
		return (GSS_S_FAILURE);
	}
	out_spcred->mcred = out_mcred;
	out_spcred->neg_mechs = GSS_C_NULL_OID_SET;
	*output_cred_handle = (gss_cred_id_t)out_spcred;

	dsyslog("Leaving spnego_gss_acquire_cred_impersonate_name\n");
	return (status);
}