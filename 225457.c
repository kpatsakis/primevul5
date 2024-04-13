get_available_mechs(OM_uint32 *minor_status,
	gss_name_t name, gss_cred_usage_t usage,
	gss_const_key_value_set_t cred_store,
	gss_cred_id_t *creds, gss_OID_set *rmechs)
{
	unsigned int	i;
	int		found = 0;
	OM_uint32 major_status = GSS_S_COMPLETE, tmpmin;
	gss_OID_set mechs, goodmechs;

	major_status = gss_indicate_mechs(minor_status, &mechs);

	if (major_status != GSS_S_COMPLETE) {
		return (major_status);
	}

	major_status = gss_create_empty_oid_set(minor_status, rmechs);

	if (major_status != GSS_S_COMPLETE) {
		(void) gss_release_oid_set(minor_status, &mechs);
		return (major_status);
	}

	for (i = 0; i < mechs->count && major_status == GSS_S_COMPLETE; i++) {
		if ((mechs->elements[i].length
		    != spnego_mechanism.mech_type.length) ||
		    memcmp(mechs->elements[i].elements,
			spnego_mechanism.mech_type.elements,
			spnego_mechanism.mech_type.length)) {

			major_status = gss_add_oid_set_member(minor_status,
							      &mechs->elements[i],
							      rmechs);
			if (major_status == GSS_S_COMPLETE)
				found++;
		}
	}

	/*
	 * If the caller wanted a list of creds returned,
	 * trim the list of mechanisms down to only those
	 * for which the creds are valid.
	 */
	if (found > 0 && major_status == GSS_S_COMPLETE && creds != NULL) {
		major_status = gss_acquire_cred_from(minor_status, name,
						     GSS_C_INDEFINITE,
						     *rmechs, usage,
						     cred_store, creds,
						     &goodmechs, NULL);

		/*
		 * Drop the old list in favor of the new
		 * "trimmed" list.
		 */
		(void) gss_release_oid_set(&tmpmin, rmechs);
		if (major_status == GSS_S_COMPLETE) {
			(void) gssint_copy_oid_set(&tmpmin,
					goodmechs, rmechs);
			(void) gss_release_oid_set(&tmpmin, &goodmechs);
		}
	}

	(void) gss_release_oid_set(&tmpmin, &mechs);
	if (found == 0 || major_status != GSS_S_COMPLETE) {
		*minor_status = ERR_SPNEGO_NO_MECHS_AVAILABLE;
		map_errcode(minor_status);
		if (major_status == GSS_S_COMPLETE)
			major_status = GSS_S_FAILURE;
	}

	return (major_status);
}