spnego_gss_inquire_cred_by_oid(
		OM_uint32 *minor_status,
		const gss_cred_id_t cred_handle,
		const gss_OID desired_object,
		gss_buffer_set_t *data_set)
{
	OM_uint32 ret;
	spnego_gss_cred_id_t spcred = (spnego_gss_cred_id_t)cred_handle;
	gss_cred_id_t mcred;
	mcred = (spcred == NULL) ? GSS_C_NO_CREDENTIAL : spcred->mcred;
	ret = gss_inquire_cred_by_oid(minor_status,
				mcred,
				desired_object,
				data_set);
	return (ret);
}