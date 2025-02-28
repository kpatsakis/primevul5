static apr_byte_t oidc_refresh_access_token(request_rec *r, oidc_cfg *c,
		oidc_session_t *session, oidc_provider_t *provider,
		char **new_access_token) {

	oidc_debug(r, "enter");

	/* get the refresh token that was stored in the session */
	const char *refresh_token = oidc_session_get_refresh_token(r, session);
	if (refresh_token == NULL) {
		oidc_warn(r,
				"refresh token routine called but no refresh_token found in the session");
		return FALSE;
	}

	/* elements returned in the refresh response */
	char *s_id_token = NULL;
	int expires_in = -1;
	char *s_token_type = NULL;
	char *s_access_token = NULL;
	char *s_refresh_token = NULL;

	/* refresh the tokens by calling the token endpoint */
	if (oidc_proto_refresh_request(r, c, provider, refresh_token, &s_id_token,
			&s_access_token, &s_token_type, &expires_in, &s_refresh_token)
			== FALSE) {
		oidc_error(r, "access_token could not be refreshed");
		return FALSE;
	}

	/* store the new access_token in the session and discard the old one */
	oidc_session_set_access_token(r, session, s_access_token);
	oidc_session_set_access_token_expires(r, session, expires_in);

	/* reset the access token refresh timestamp */
	oidc_session_reset_access_token_last_refresh(r, session);

	/* see if we need to return it as a parameter */
	if (new_access_token != NULL)
		*new_access_token = s_access_token;

	/* if we have a new refresh token (rolling refresh), store it in the session and overwrite the old one */
	if (s_refresh_token != NULL)
		oidc_session_set_refresh_token(r, session, s_refresh_token);

	return TRUE;
}