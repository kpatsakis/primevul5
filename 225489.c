init_ctx_call_init(OM_uint32 *minor_status,
		   spnego_gss_ctx_id_t sc,
		   spnego_gss_cred_id_t spcred,
		   gss_name_t target_name,
		   OM_uint32 req_flags,
		   OM_uint32 time_req,
		   gss_buffer_t mechtok_in,
		   gss_OID *actual_mech,
		   gss_buffer_t mechtok_out,
		   OM_uint32 *ret_flags,
		   OM_uint32 *time_rec,
		   OM_uint32 *negState,
		   send_token_flag *send_token)
{
	OM_uint32 ret, tmpret, tmpmin;
	gss_cred_id_t mcred;

	mcred = (spcred == NULL) ? GSS_C_NO_CREDENTIAL : spcred->mcred;
	ret = gss_init_sec_context(minor_status,
				   mcred,
				   &sc->ctx_handle,
				   target_name,
				   sc->internal_mech,
				   (req_flags | GSS_C_INTEG_FLAG),
				   time_req,
				   GSS_C_NO_CHANNEL_BINDINGS,
				   mechtok_in,
				   &sc->actual_mech,
				   mechtok_out,
				   &sc->ctx_flags,
				   time_rec);
	if (ret == GSS_S_COMPLETE) {
		sc->mech_complete = 1;
		if (ret_flags != NULL)
			*ret_flags = sc->ctx_flags;
		/*
		 * Microsoft SPNEGO implementations expect an even number of
		 * token exchanges.  So if we're sending a final token, ask for
		 * a zero-length token back from the server.  Also ask for a
		 * token back if this is the first token or if a MIC exchange
		 * is required.
		 */
		if (*send_token == CONT_TOKEN_SEND &&
		    mechtok_out->length == 0 &&
		    (!sc->mic_reqd ||
		     !(sc->ctx_flags & GSS_C_INTEG_FLAG))) {
			/* The exchange is complete. */
			*negState = ACCEPT_COMPLETE;
			ret = GSS_S_COMPLETE;
			*send_token = NO_TOKEN_SEND;
		} else {
			/* Ask for one more hop. */
			*negState = ACCEPT_INCOMPLETE;
			ret = GSS_S_CONTINUE_NEEDED;
		}
		return ret;
	}

	if (ret == GSS_S_CONTINUE_NEEDED)
		return ret;

	if (*send_token != INIT_TOKEN_SEND) {
		*send_token = ERROR_TOKEN_SEND;
		*negState = REJECT;
		return ret;
	}

	/*
	 * Since this is the first token, we can fall back to later mechanisms
	 * in the list.  Since the mechanism list is expected to be short, we
	 * can do this with recursion.  If all mechanisms produce errors, the
	 * caller should get the error from the first mech in the list.
	 */
	gssalloc_free(sc->mech_set->elements->elements);
	memmove(sc->mech_set->elements, sc->mech_set->elements + 1,
		--sc->mech_set->count * sizeof(*sc->mech_set->elements));
	if (sc->mech_set->count == 0)
		goto fail;
	gss_release_buffer(&tmpmin, &sc->DER_mechTypes);
	if (put_mech_set(sc->mech_set, &sc->DER_mechTypes) < 0)
		goto fail;
	tmpret = init_ctx_call_init(&tmpmin, sc, spcred, target_name,
				    req_flags, time_req, mechtok_in,
				    actual_mech, mechtok_out, ret_flags,
				    time_rec, negState, send_token);
	if (HARD_ERROR(tmpret))
		goto fail;
	*minor_status = tmpmin;
	return tmpret;

fail:
	/* Don't output token on error from first call. */
	*send_token = NO_TOKEN_SEND;
	*negState = REJECT;
	return ret;
}