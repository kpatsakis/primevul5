spnego_gss_import_cred(OM_uint32 *minor_status,
		       gss_buffer_t token,
		       gss_cred_id_t *cred_handle)
{
	OM_uint32 ret;
	spnego_gss_cred_id_t spcred;
	gss_cred_id_t mcred;

	ret = gss_import_cred(minor_status, token, &mcred);
	if (GSS_ERROR(ret))
		return (ret);
	spcred = malloc(sizeof(*spcred));
	if (spcred == NULL) {
		gss_release_cred(minor_status, &mcred);
		*minor_status = ENOMEM;
		return (GSS_S_FAILURE);
	}
	spcred->mcred = mcred;
	spcred->neg_mechs = GSS_C_NULL_OID_SET;
	*cred_handle = (gss_cred_id_t)spcred;
	return (ret);
}