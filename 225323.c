static void oidc_copy_tokens_to_request_state(request_rec *r,
		oidc_session_t *session, const char **s_id_token, const char **s_claims) {

	const char *id_token = oidc_session_get_idtoken_claims(r, session);
	const char *claims = oidc_session_get_userinfo_claims(r, session);

	oidc_debug(r, "id_token=%s claims=%s", id_token, claims);

	if (id_token != NULL) {
		oidc_request_state_set(r, OIDC_REQUEST_STATE_KEY_IDTOKEN, id_token);
		if (s_id_token != NULL)
			*s_id_token = id_token;
	}

	if (claims != NULL) {
		oidc_request_state_set(r, OIDC_REQUEST_STATE_KEY_CLAIMS, claims);
		if (s_claims != NULL)
			*s_claims = claims;
	}
}