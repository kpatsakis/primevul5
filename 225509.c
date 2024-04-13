spnego_gss_set_neg_mechs(OM_uint32 *minor_status,
			 gss_cred_id_t cred_handle,
			 const gss_OID_set mech_list)
{
	OM_uint32 ret;
	spnego_gss_cred_id_t spcred = (spnego_gss_cred_id_t)cred_handle;

	/* Store mech_list in spcred for use in negotiation logic. */
	gss_release_oid_set(minor_status, &spcred->neg_mechs);
	ret = generic_gss_copy_oid_set(minor_status, mech_list,
				       &spcred->neg_mechs);
	return (ret);
}