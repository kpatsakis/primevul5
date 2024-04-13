static apr_byte_t oidc_session_pass_tokens_and_save(request_rec *r,
		oidc_cfg *cfg, oidc_session_t *session, apr_byte_t needs_save) {

	apr_byte_t pass_headers = oidc_cfg_dir_pass_info_in_headers(r);
	apr_byte_t pass_envvars = oidc_cfg_dir_pass_info_in_envvars(r);

	/* set the refresh_token in the app headers/variables, if enabled for this location/directory */
	const char *refresh_token = oidc_session_get_refresh_token(r, session);
	if ((oidc_cfg_dir_pass_refresh_token(r) != 0) && (refresh_token != NULL)) {
		/* pass it to the app in a header or environment variable */
		oidc_util_set_app_info(r, OIDC_APP_INFO_REFRESH_TOKEN, refresh_token,
				OIDC_DEFAULT_HEADER_PREFIX, pass_headers, pass_envvars);
	}

	/* set the access_token in the app headers/variables */
	const char *access_token = oidc_session_get_access_token(r, session);
	if (access_token != NULL) {
		/* pass it to the app in a header or environment variable */
		oidc_util_set_app_info(r, OIDC_APP_INFO_ACCESS_TOKEN, access_token,
				OIDC_DEFAULT_HEADER_PREFIX, pass_headers, pass_envvars);
	}

	/* set the expiry timestamp in the app headers/variables */
	const char *access_token_expires = oidc_session_get_access_token_expires(r,
			session);
	if (access_token_expires != NULL) {
		/* pass it to the app in a header or environment variable */
		oidc_util_set_app_info(r, OIDC_APP_INFO_ACCESS_TOKEN_EXP,
				access_token_expires,
				OIDC_DEFAULT_HEADER_PREFIX, pass_headers, pass_envvars);
	}

	/*
	 * reset the session inactivity timer
	 * but only do this once per 10% of the inactivity timeout interval (with a max to 60 seconds)
	 * for performance reasons
	 *
	 * now there's a small chance that the session ends 10% (or a minute) earlier than configured/expected
	 * cq. when there's a request after a recent save (so no update) and then no activity happens until
	 * a request comes in just before the session should expire
	 * ("recent" and "just before" refer to 10%-with-a-max-of-60-seconds of the inactivity interval after
	 * the start/last-update and before the expiry of the session respectively)
	 *
	 * this is be deemed acceptable here because of performance gain
	 */
	apr_time_t interval = apr_time_from_sec(cfg->session_inactivity_timeout);
	apr_time_t now = apr_time_now();
	apr_time_t slack = interval / 10;
	if (slack > apr_time_from_sec(60))
		slack = apr_time_from_sec(60);
	if (session->expiry - now < interval - slack) {
		session->expiry = now + interval;
		needs_save = TRUE;
	}

	/* log message about session expiry */
	oidc_log_session_expires(r, "session inactivity timeout", session->expiry);

	/* check if something was updated in the session and we need to save it again */
	if (needs_save)
		if (oidc_session_save(r, session, FALSE) == FALSE)
			return FALSE;

	return TRUE;
}