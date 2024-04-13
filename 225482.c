spnego_gss_set_cred_option(
		OM_uint32 *minor_status,
		gss_cred_id_t *cred_handle,
		const gss_OID desired_object,
		const gss_buffer_t value)
{
	OM_uint32 ret;
	OM_uint32 tmp_minor_status;
	spnego_gss_cred_id_t spcred = (spnego_gss_cred_id_t)*cred_handle;
	gss_cred_id_t mcred;

	mcred = (spcred == NULL) ? GSS_C_NO_CREDENTIAL : spcred->mcred;
	ret = gss_set_cred_option(minor_status,
				  &mcred,
				  desired_object,
				  value);
	if (ret == GSS_S_COMPLETE && spcred == NULL) {
		/*
		 * If the mechanism allocated a new credential handle, then
		 * we need to wrap it up in an SPNEGO credential handle.
		 */

		spcred = malloc(sizeof(spnego_gss_cred_id_rec));
		if (spcred == NULL) {
			gss_release_cred(&tmp_minor_status, &mcred);
			*minor_status = ENOMEM;
			return (GSS_S_FAILURE);
		}
		spcred->mcred = mcred;
		spcred->neg_mechs = GSS_C_NULL_OID_SET;
		*cred_handle = (gss_cred_id_t)spcred;
	}

	return (ret);
}