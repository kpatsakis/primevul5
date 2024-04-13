static void oidc_store_userinfo_claims(request_rec *r, oidc_cfg *c,
		oidc_session_t *session, oidc_provider_t *provider, const char *claims,
		const char *userinfo_jwt) {

	oidc_debug(r, "enter");

	/* see if we've resolved any claims */
	if (claims != NULL) {
		/*
		 * Successfully decoded a set claims from the response so we can store them
		 * (well actually the stringified representation in the response)
		 * in the session context safely now
		 */
		oidc_session_set_userinfo_claims(r, session, claims);

		if (c->session_type != OIDC_SESSION_TYPE_CLIENT_COOKIE) {
			/* this will also clear the entry if a JWT was not returned at this point */
			oidc_session_set_userinfo_jwt(r, session, userinfo_jwt);
		}

	} else {
		/*
		 * clear the existing claims because we could not refresh them
		 */
		oidc_session_set_userinfo_claims(r, session, NULL);

		oidc_session_set_userinfo_jwt(r, session, NULL);
	}

	/* store the last refresh time if we've configured a userinfo refresh interval */
	if (provider->userinfo_refresh_interval > 0)
		oidc_session_reset_userinfo_last_refresh(r, session);
}