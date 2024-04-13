init_ctx_nego(OM_uint32 *minor_status, spnego_gss_ctx_id_t sc,
	      OM_uint32 acc_negState, gss_OID supportedMech,
	      gss_buffer_t *responseToken, gss_buffer_t *mechListMIC,
	      OM_uint32 *negState, send_token_flag *tokflag)
{
	OM_uint32 ret;

	*negState = REJECT;
	*tokflag = ERROR_TOKEN_SEND;
	ret = GSS_S_DEFECTIVE_TOKEN;
	/*
	 * Both supportedMech and negState must be present in first
	 * acceptor token.
	 */
	if (supportedMech == GSS_C_NO_OID) {
		*minor_status = ERR_SPNEGO_NO_MECH_FROM_ACCEPTOR;
		map_errcode(minor_status);
		return GSS_S_DEFECTIVE_TOKEN;
	}
	if (acc_negState == ACCEPT_DEFECTIVE_TOKEN) {
		*minor_status = ERR_SPNEGO_NEGOTIATION_FAILED;
		map_errcode(minor_status);
		return GSS_S_DEFECTIVE_TOKEN;
	}

	/*
	 * If the mechanism we sent is not the mechanism returned from
	 * the server, we need to handle the server's counter
	 * proposal.  There is a bug in SAMBA servers that always send
	 * the old Kerberos mech OID, even though we sent the new one.
	 * So we will treat all the Kerberos mech OIDS as the same.
         */
	if (!(is_kerb_mech(supportedMech) &&
	      is_kerb_mech(sc->internal_mech)) &&
	    !g_OID_equal(supportedMech, sc->internal_mech)) {
		ret = init_ctx_reselect(minor_status, sc,
					acc_negState, supportedMech,
					responseToken, mechListMIC,
					negState, tokflag);

	} else if (*responseToken == GSS_C_NO_BUFFER) {
		if (sc->mech_complete) {
			/*
			 * Mech completed on first call to its
			 * init_sec_context().  Acceptor sends no mech
			 * token.
			 */
			*negState = ACCEPT_COMPLETE;
			*tokflag = NO_TOKEN_SEND;
			ret = GSS_S_COMPLETE;
		} else {
			/*
			 * Reject missing mech token when optimistic
			 * mech selected.
			 */
			*minor_status = ERR_SPNEGO_NO_TOKEN_FROM_ACCEPTOR;
			map_errcode(minor_status);
			ret = GSS_S_DEFECTIVE_TOKEN;
		}
	} else if ((*responseToken)->length == 0 && sc->mech_complete) {
		/* Handle old IIS servers returning empty token instead of
		 * null tokens in the non-mutual auth case. */
		*negState = ACCEPT_COMPLETE;
		*tokflag = NO_TOKEN_SEND;
		ret = GSS_S_COMPLETE;
	} else if (sc->mech_complete) {
		/* Reject spurious mech token. */
		ret = GSS_S_DEFECTIVE_TOKEN;
	} else {
		*negState = ACCEPT_INCOMPLETE;
		*tokflag = CONT_TOKEN_SEND;
		ret = GSS_S_CONTINUE_NEEDED;
	}
	sc->nego_done = 1;
	return ret;
}