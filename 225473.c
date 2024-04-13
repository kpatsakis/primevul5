acc_ctx_call_acc(OM_uint32 *minor_status, spnego_gss_ctx_id_t sc,
		 spnego_gss_cred_id_t spcred, gss_buffer_t mechtok_in,
		 gss_OID *mech_type, gss_buffer_t mechtok_out,
		 OM_uint32 *ret_flags, OM_uint32 *time_rec,
		 gss_cred_id_t *delegated_cred_handle,
		 OM_uint32 *negState, send_token_flag *tokflag)
{
	OM_uint32 ret;
	gss_OID_desc mechoid;
	gss_cred_id_t mcred;

	if (sc->ctx_handle == GSS_C_NO_CONTEXT) {
		/*
		 * mechoid is an alias; don't free it.
		 */
		ret = gssint_get_mech_type(&mechoid, mechtok_in);
		if (ret != GSS_S_COMPLETE) {
			*tokflag = NO_TOKEN_SEND;
			return ret;
		}
		ret = acc_ctx_vfy_oid(minor_status, sc, &mechoid,
				      negState, tokflag);
		if (ret != GSS_S_COMPLETE)
			return ret;
	}

	mcred = (spcred == NULL) ? GSS_C_NO_CREDENTIAL : spcred->mcred;
	ret = gss_accept_sec_context(minor_status,
				     &sc->ctx_handle,
				     mcred,
				     mechtok_in,
				     GSS_C_NO_CHANNEL_BINDINGS,
				     &sc->internal_name,
				     mech_type,
				     mechtok_out,
				     &sc->ctx_flags,
				     time_rec,
				     delegated_cred_handle);
	if (ret == GSS_S_COMPLETE) {
#ifdef MS_BUG_TEST
		/*
		 * Force MIC to be not required even if we previously
		 * requested a MIC.
		 */
		char *envstr = getenv("MS_FORCE_NO_MIC");

		if (envstr != NULL && strcmp(envstr, "1") == 0 &&
		    !(sc->ctx_flags & GSS_C_MUTUAL_FLAG) &&
		    sc->mic_reqd) {

			sc->mic_reqd = 0;
		}
#endif
		sc->mech_complete = 1;
		if (ret_flags != NULL)
			*ret_flags = sc->ctx_flags;

		if (!sc->mic_reqd ||
		    !(sc->ctx_flags & GSS_C_INTEG_FLAG)) {
			/* No MIC exchange required, so we're done. */
			*negState = ACCEPT_COMPLETE;
			ret = GSS_S_COMPLETE;
		} else {
			/* handle_mic will decide if we're done. */
			ret = GSS_S_CONTINUE_NEEDED;
		}
	} else if (ret != GSS_S_CONTINUE_NEEDED) {
		*negState = REJECT;
		*tokflag = ERROR_TOKEN_SEND;
	}
	return ret;
}