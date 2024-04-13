static int oidc_handle_session_management(request_rec *r, oidc_cfg *c,
		oidc_session_t *session) {
	char *cmd = NULL;
	const char *id_token_hint = NULL;
	oidc_provider_t *provider = NULL;

	/* get the command passed to the session management handler */
	oidc_util_get_request_parameter(r, OIDC_REDIRECT_URI_REQUEST_SESSION, &cmd);
	if (cmd == NULL) {
		oidc_error(r, "session management handler called with no command");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	/* see if this is a local logout during session management */
	if (apr_strnatcmp("logout", cmd) == 0) {
		oidc_debug(r,
				"[session=logout] calling oidc_handle_logout_request because of session mgmt local logout call.");
		return oidc_handle_logout_request(r, c, session, c->default_slo_url);
	}

	oidc_get_provider_from_session(r, c, session, &provider);

	/* see if this is a request for the OP iframe */
	if (apr_strnatcmp("iframe_op", cmd) == 0) {
		if (provider->check_session_iframe != NULL) {
			return oidc_handle_session_management_iframe_op(r, c, session,
					provider->check_session_iframe);
		}
		return HTTP_NOT_FOUND;
	}

	/* see if this is a request for the RP iframe */
	if (apr_strnatcmp("iframe_rp", cmd) == 0) {
		if ((provider->client_id != NULL)
				&& (provider->check_session_iframe != NULL)) {
			return oidc_handle_session_management_iframe_rp(r, c, session,
					provider->client_id, provider->check_session_iframe);
		}
		oidc_debug(r,
				"iframe_rp command issued but no client (%s) and/or no check_session_iframe (%s) set",
				provider->client_id, provider->check_session_iframe);
		return HTTP_NOT_FOUND;
	}

	/* see if this is a request check the login state with the OP */
	if (apr_strnatcmp("check", cmd) == 0) {
		id_token_hint = oidc_session_get_idtoken(r, session);
		if ((session->remote_user != NULL) && (provider != NULL)) {
			/*
			 * TODO: this doesn't work with per-path provided auth_request_params and scopes
			 *       as oidc_dir_cfg_path_auth_request_params and oidc_dir_cfg_path_scope will pick
			 *       those for the redirect_uri itself; do we need to store those as part of the
			 *       session now?
			 */
			return oidc_authenticate_user(r, c, provider,
					apr_psprintf(r->pool, "%s?session=iframe_rp",
							oidc_get_redirect_uri_iss(r, c, provider)), NULL,
							id_token_hint, "none",
							oidc_dir_cfg_path_auth_request_params(r),
							oidc_dir_cfg_path_scope(r));
		}
		oidc_debug(r,
				"[session=check] calling oidc_handle_logout_request because no session found.");
		return oidc_session_redirect_parent_window_to_logout(r, c);
	}

	/* handle failure in fallthrough */
	oidc_error(r, "unknown command: %s", cmd);

	return HTTP_INTERNAL_SERVER_ERROR;
}