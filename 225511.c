spnego_gss_release_cred(OM_uint32 *minor_status,
			gss_cred_id_t *cred_handle)
{
	spnego_gss_cred_id_t spcred = NULL;

	dsyslog("Entering spnego_gss_release_cred\n");

	if (minor_status == NULL || cred_handle == NULL)
		return (GSS_S_CALL_INACCESSIBLE_WRITE);

	*minor_status = 0;

	if (*cred_handle == GSS_C_NO_CREDENTIAL)
		return (GSS_S_COMPLETE);

	spcred = (spnego_gss_cred_id_t)*cred_handle;
	*cred_handle = GSS_C_NO_CREDENTIAL;
	gss_release_oid_set(minor_status, &spcred->neg_mechs);
	gss_release_cred(minor_status, &spcred->mcred);
	free(spcred);

	dsyslog("Leaving spnego_gss_release_cred\n");
	return (GSS_S_COMPLETE);
}