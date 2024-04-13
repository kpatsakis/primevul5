static apr_byte_t oidc_save_in_session(request_rec *r, oidc_cfg *c,
		oidc_session_t *session, oidc_provider_t *provider,
		const char *remoteUser, const char *id_token, oidc_jwt_t *id_token_jwt,
		const char *claims, const char *access_token, const int expires_in,
		const char *refresh_token, const char *session_state, const char *state,
		const char *original_url, const char *userinfo_jwt) {

	/* store the user in the session */
	session->remote_user = remoteUser;

	/* set the session expiry to the inactivity timeout */
	session->expiry =
			apr_time_now() + apr_time_from_sec(c->session_inactivity_timeout);

	/* store the claims payload in the id_token for later reference */
	oidc_session_set_idtoken_claims(r, session,
			id_token_jwt->payload.value.str);

	if (c->session_type != OIDC_SESSION_TYPE_CLIENT_COOKIE) {
		/* store the compact serialized representation of the id_token for later reference  */
		oidc_session_set_idtoken(r, session, id_token);
	}

	/* store the issuer in the session (at least needed for session mgmt and token refresh */
	oidc_session_set_issuer(r, session, provider->issuer);

	/* store the state and original URL in the session for handling browser-back more elegantly */
	oidc_session_set_request_state(r, session, state);
	oidc_session_set_original_url(r, session, original_url);

	if ((session_state != NULL) && (provider->check_session_iframe != NULL)) {
		/* store the session state and required parameters session management  */
		oidc_session_set_session_state(r, session, session_state);
		oidc_debug(r,
				"session management enabled: stored session_state (%s), check_session_iframe (%s) and client_id (%s) in the session",
				session_state, provider->check_session_iframe,
				provider->client_id);
	} else if (provider->check_session_iframe == NULL) {
		oidc_debug(r,
				"session management disabled: \"check_session_iframe\" is not set in provider configuration");
	} else {
		oidc_debug(r,
				"session management disabled: no \"session_state\" value is provided in the authentication response even though \"check_session_iframe\" (%s) is set in the provider configuration",
				provider->check_session_iframe);
	}

	/* store claims resolved from userinfo endpoint */
	oidc_store_userinfo_claims(r, c, session, provider, claims, userinfo_jwt);

	/* see if we have an access_token */
	if (access_token != NULL) {
		/* store the access_token in the session context */
		oidc_session_set_access_token(r, session, access_token);
		/* store the associated expires_in value */
		oidc_session_set_access_token_expires(r, session, expires_in);
		/* reset the access token refresh timestamp */
		oidc_session_reset_access_token_last_refresh(r, session);
	}

	/* see if we have a refresh_token */
	if (refresh_token != NULL) {
		/* store the refresh_token in the session context */
		oidc_session_set_refresh_token(r, session, refresh_token);
	}

	/* store max session duration in the session as a hard cut-off expiry timestamp */
	apr_time_t session_expires =
			(provider->session_max_duration == 0) ?
					apr_time_from_sec(id_token_jwt->payload.exp) :
					(apr_time_now()
							+ apr_time_from_sec(provider->session_max_duration));
	oidc_session_set_session_expires(r, session, session_expires);

	oidc_debug(r,
			"provider->session_max_duration = %d, session_expires=%" APR_TIME_T_FMT,
			provider->session_max_duration, session_expires);

	/* log message about max session duration */
	oidc_log_session_expires(r, "session max lifetime", session_expires);

	/* store the domain for which this session is valid */
	oidc_session_set_cookie_domain(r, session,
			c->cookie_domain ? c->cookie_domain : oidc_get_current_url_host(r));

	char *sid = NULL;
	oidc_debug(r, "provider->backchannel_logout_supported=%d",
			provider->backchannel_logout_supported);
	if (provider->backchannel_logout_supported > 0) {
		oidc_jose_get_string(r->pool, id_token_jwt->payload.value.json,
				OIDC_CLAIM_SID, FALSE, &sid, NULL);
		if (sid == NULL)
			sid = id_token_jwt->payload.sub;
		session->sid = oidc_make_sid_iss_unique(r, sid, provider->issuer);
	}

	/* store the session */
	return oidc_session_save(r, session, TRUE);
}