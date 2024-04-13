spnego_gss_acquire_cred_with_password(OM_uint32 *minor_status,
				      const gss_name_t desired_name,
				      const gss_buffer_t password,
				      OM_uint32 time_req,
				      const gss_OID_set desired_mechs,
				      gss_cred_usage_t cred_usage,
				      gss_cred_id_t *output_cred_handle,
				      gss_OID_set *actual_mechs,
				      OM_uint32 *time_rec)
{
	OM_uint32 status, tmpmin;
	gss_OID_set amechs = GSS_C_NULL_OID_SET;
	gss_cred_id_t mcred = NULL;
	spnego_gss_cred_id_t spcred = NULL;

	dsyslog("Entering spnego_gss_acquire_cred_with_password\n");

	if (actual_mechs)
		*actual_mechs = NULL;

	if (time_rec)
		*time_rec = 0;

	status = get_available_mechs(minor_status, desired_name,
				     cred_usage, GSS_C_NO_CRED_STORE,
				     NULL, &amechs);
	if (status != GSS_S_COMPLETE)
	    goto cleanup;

	status = gss_acquire_cred_with_password(minor_status, desired_name,
						password, time_req, amechs,
						cred_usage, &mcred,
						actual_mechs, time_rec);
	if (status != GSS_S_COMPLETE)
	    goto cleanup;

	spcred = malloc(sizeof(spnego_gss_cred_id_rec));
	if (spcred == NULL) {
		*minor_status = ENOMEM;
		status = GSS_S_FAILURE;
		goto cleanup;
	}
	spcred->neg_mechs = GSS_C_NULL_OID_SET;
	spcred->mcred = mcred;
	mcred = GSS_C_NO_CREDENTIAL;
	*output_cred_handle = (gss_cred_id_t)spcred;

cleanup:

	(void) gss_release_oid_set(&tmpmin, &amechs);
	(void) gss_release_cred(&tmpmin, &mcred);

	dsyslog("Leaving spnego_gss_acquire_cred_with_password\n");
	return (status);
}