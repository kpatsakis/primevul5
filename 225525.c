spnego_gss_export_cred(OM_uint32 *minor_status,
		       gss_cred_id_t cred_handle,
		       gss_buffer_t token)
{
	spnego_gss_cred_id_t spcred = (spnego_gss_cred_id_t)cred_handle;

	return (gss_export_cred(minor_status, spcred->mcred, token));
}