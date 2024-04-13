process_mic(OM_uint32 *minor_status, gss_buffer_t mic_in,
	    spnego_gss_ctx_id_t sc, gss_buffer_t *mic_out,
	    OM_uint32 *negState, send_token_flag *tokflag)
{
	OM_uint32 ret, tmpmin;
	gss_qop_t qop_state;
	gss_buffer_desc tmpmic = GSS_C_EMPTY_BUFFER;

	ret = GSS_S_FAILURE;
	if (mic_in != GSS_C_NO_BUFFER) {
		ret = gss_verify_mic(minor_status, sc->ctx_handle,
				     &sc->DER_mechTypes,
				     mic_in, &qop_state);
		if (ret != GSS_S_COMPLETE) {
			*negState = REJECT;
			*tokflag = ERROR_TOKEN_SEND;
			return ret;
		}
		/* If we got a MIC, we must send a MIC. */
		sc->mic_reqd = 1;
		sc->mic_rcvd = 1;
	}
	if (sc->mic_reqd && !sc->mic_sent) {
		ret = gss_get_mic(minor_status, sc->ctx_handle,
				  GSS_C_QOP_DEFAULT,
				  &sc->DER_mechTypes,
				  &tmpmic);
		if (ret != GSS_S_COMPLETE) {
			gss_release_buffer(&tmpmin, &tmpmic);
			*tokflag = NO_TOKEN_SEND;
			return ret;
		}
		*mic_out = malloc(sizeof(gss_buffer_desc));
		if (*mic_out == GSS_C_NO_BUFFER) {
			gss_release_buffer(&tmpmin, &tmpmic);
			*tokflag = NO_TOKEN_SEND;
			return GSS_S_FAILURE;
		}
		**mic_out = tmpmic;
		sc->mic_sent = 1;
	}
	return GSS_S_COMPLETE;
}