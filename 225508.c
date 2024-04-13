get_negotiable_mechs(OM_uint32 *minor_status, spnego_gss_cred_id_t spcred,
		     gss_cred_usage_t usage, gss_OID_set *rmechs)
{
	OM_uint32 ret, tmpmin;
	gss_cred_id_t creds = GSS_C_NO_CREDENTIAL, *credptr;
	gss_OID_set cred_mechs = GSS_C_NULL_OID_SET;
	gss_OID_set intersect_mechs = GSS_C_NULL_OID_SET;
	unsigned int i;
	int present;

	if (spcred == NULL) {
		/*
		 * The default credentials were supplied.  Return a list of all
		 * available mechs except SPNEGO.  When initiating, trim this
		 * list to mechs we can acquire credentials for.
		 */
		credptr = (usage == GSS_C_INITIATE) ? &creds : NULL;
		ret = get_available_mechs(minor_status, GSS_C_NO_NAME, usage,
					  GSS_C_NO_CRED_STORE, credptr,
					  rmechs);
		gss_release_cred(&tmpmin, &creds);
		return (ret);
	}

	/* Get the list of mechs in the mechglue cred. */
	ret = gss_inquire_cred(minor_status, spcred->mcred, NULL, NULL, NULL,
			       &cred_mechs);
	if (ret != GSS_S_COMPLETE)
		return (ret);

	if (spcred->neg_mechs == GSS_C_NULL_OID_SET) {
		/* gss_set_neg_mechs was never called; return cred_mechs. */
		*rmechs = cred_mechs;
		*minor_status = 0;
		return (GSS_S_COMPLETE);
	}

	/* Compute the intersection of cred_mechs and spcred->neg_mechs,
	 * preserving the order in spcred->neg_mechs. */
	ret = gss_create_empty_oid_set(minor_status, &intersect_mechs);
	if (ret != GSS_S_COMPLETE) {
		gss_release_oid_set(&tmpmin, &cred_mechs);
		return (ret);
	}

	for (i = 0; i < spcred->neg_mechs->count; i++) {
		gss_test_oid_set_member(&tmpmin,
					&spcred->neg_mechs->elements[i],
					cred_mechs, &present);
		if (!present)
			continue;
		ret = gss_add_oid_set_member(minor_status,
					     &spcred->neg_mechs->elements[i],
					     &intersect_mechs);
		if (ret != GSS_S_COMPLETE)
			break;
	}

	gss_release_oid_set(&tmpmin, &cred_mechs);
	if (intersect_mechs->count == 0 || ret != GSS_S_COMPLETE) {
		gss_release_oid_set(&tmpmin, &intersect_mechs);
		*minor_status = ERR_SPNEGO_NO_MECHS_AVAILABLE;
		map_errcode(minor_status);
		return (GSS_S_FAILURE);
	}

	*rmechs = intersect_mechs;
	*minor_status = 0;
	return (GSS_S_COMPLETE);
}