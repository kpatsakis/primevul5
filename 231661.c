static apr_byte_t oidc_authorization_response_match_state(request_rec *r,
		oidc_cfg *c, const char *state, struct oidc_provider_t **provider,
		oidc_proto_state_t **proto_state) {

	oidc_debug(r, "enter (state=%s)", state);

	if ((state == NULL) || (apr_strnatcmp(state, "") == 0)) {
		oidc_error(r, "state parameter is not set");
		return FALSE;
	}

	/* check the state parameter against what we stored in a cookie */
	if (oidc_restore_proto_state(r, c, state, proto_state) == FALSE) {
		oidc_error(r, "unable to restore state");
		return FALSE;
	}

	*provider = oidc_get_provider_for_issuer(r, c,
			oidc_proto_state_get_issuer(*proto_state), FALSE);

	return (*provider != NULL);
}