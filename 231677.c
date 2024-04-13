static apr_byte_t oidc_refresh_access_token_before_expiry(request_rec *r,
		oidc_cfg *cfg, oidc_session_t *session, int ttl_minimum, int logout_on_error) {

	const char *s_access_token_expires = NULL;
	apr_time_t t_expires = -1;
	oidc_provider_t *provider = NULL;

	oidc_debug(r, "ttl_minimum=%d", ttl_minimum);

	if (ttl_minimum < 0)
		return FALSE;

	s_access_token_expires = oidc_session_get_access_token_expires(r, session);
	if (s_access_token_expires == NULL) {
		oidc_debug(r,
				"no access token expires_in stored in the session (i.e. returned from in the authorization response), so cannot refresh the access token based on TTL requirement");
		return FALSE;
	}

	if (oidc_session_get_refresh_token(r, session) == NULL) {
		oidc_debug(r,
				"no refresh token stored in the session, so cannot refresh the access token based on TTL requirement");
		return FALSE;
	}

	if (sscanf(s_access_token_expires, "%" APR_TIME_T_FMT, &t_expires) != 1) {
		oidc_error(r, "could not parse s_access_token_expires %s",
				s_access_token_expires);
		return FALSE;
	}

	t_expires = apr_time_from_sec(t_expires - ttl_minimum);

	oidc_debug(r, "refresh needed in: %" APR_TIME_T_FMT " seconds",
			apr_time_sec(t_expires - apr_time_now()));

	if (t_expires > apr_time_now())
		return FALSE;

	if (oidc_get_provider_from_session(r, cfg, session, &provider) == FALSE)
		return FALSE;

	if (oidc_refresh_access_token(r, cfg, session, provider,
			NULL) == FALSE) {
		oidc_warn(r, "access_token could not be refreshed, logout=%d", logout_on_error & OIDC_LOGOUT_ON_ERROR_REFRESH);
		if (logout_on_error & OIDC_LOGOUT_ON_ERROR_REFRESH)
			return ERROR;
		else
			return FALSE;
	}

	return TRUE;
}