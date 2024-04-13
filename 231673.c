static int oidc_handle_info_request(request_rec *r, oidc_cfg *c,
		oidc_session_t *session) {
	int rc = HTTP_UNAUTHORIZED;
	apr_byte_t needs_save = FALSE;
	char *s_format = NULL, *s_interval = NULL, *r_value = NULL;
	oidc_util_get_request_parameter(r, OIDC_REDIRECT_URI_REQUEST_INFO,
			&s_format);
	oidc_util_get_request_parameter(r,
			OIDC_INFO_PARAM_ACCESS_TOKEN_REFRESH_INTERVAL, &s_interval);

	/* see if this is a request for a format that is supported */
	if ((apr_strnatcmp(OIDC_HOOK_INFO_FORMAT_JSON, s_format) != 0)
			&& (apr_strnatcmp(OIDC_HOOK_INFO_FORMAT_HTML, s_format) != 0)) {
		oidc_warn(r, "request for unknown format: %s", s_format);
		return HTTP_UNSUPPORTED_MEDIA_TYPE;
	}

	/* check that we actually have a user session and this is someone calling with a proper session cookie */
	if (session->remote_user == NULL) {
		oidc_warn(r, "no user session found");
		return HTTP_UNAUTHORIZED;
	}

	/* set the user in the main request for further (incl. sub-request and authz) processing */
	r->user = apr_pstrdup(r->pool, session->remote_user);

	if (c->info_hook_data == NULL) {
		oidc_warn(r, "no data configured to return in " OIDCInfoHook);
		return HTTP_NOT_FOUND;
	}

	/* see if we can and need to refresh the access token */
	if ((s_interval != NULL)
			&& (oidc_session_get_refresh_token(r, session) != NULL)) {

		apr_time_t t_interval;
		if (sscanf(s_interval, "%" APR_TIME_T_FMT, &t_interval) == 1) {
			t_interval = apr_time_from_sec(t_interval);

			/* get the last refresh timestamp from the session info */
			apr_time_t last_refresh =
					oidc_session_get_access_token_last_refresh(r, session);

			oidc_debug(r, "refresh needed in: %" APR_TIME_T_FMT " seconds",
					apr_time_sec(last_refresh + t_interval - apr_time_now()));

			/* see if we need to refresh again */
			if (last_refresh + t_interval < apr_time_now()) {

				/* get the current provider info */
				oidc_provider_t *provider = NULL;
				if (oidc_get_provider_from_session(r, c, session,
						&provider) == FALSE)
					return HTTP_INTERNAL_SERVER_ERROR;

				/* execute the actual refresh grant */
				if (oidc_refresh_access_token(r, c, session, provider,
						NULL) == FALSE)
					oidc_warn(r, "access_token could not be refreshed");
				else
					needs_save = TRUE;
			}
		}
	}

	/* create the JSON object */
	json_t *json = json_object();

	/* add a timestamp of creation in there for the caller */
	if (apr_hash_get(c->info_hook_data, OIDC_HOOK_INFO_TIMESTAMP,
			APR_HASH_KEY_STRING)) {
		json_object_set_new(json, OIDC_HOOK_INFO_TIMESTAMP,
				json_integer(apr_time_sec(apr_time_now())));
	}

	/*
	 * refresh the claims from the userinfo endpoint
	 * side-effect is that this may refresh the access token if not already done
	 * note that OIDCUserInfoRefreshInterval should be set to control the refresh policy
	 */
	needs_save |= oidc_refresh_claims_from_userinfo_endpoint(r, c, session);

	/* include the access token in the session info */
	if (apr_hash_get(c->info_hook_data, OIDC_HOOK_INFO_ACCES_TOKEN,
			APR_HASH_KEY_STRING)) {
		const char *access_token = oidc_session_get_access_token(r, session);
		if (access_token != NULL)
			json_object_set_new(json, OIDC_HOOK_INFO_ACCES_TOKEN,
					json_string(access_token));
	}

	/* include the access token expiry timestamp in the session info */
	if (apr_hash_get(c->info_hook_data, OIDC_HOOK_INFO_ACCES_TOKEN_EXP,
			APR_HASH_KEY_STRING)) {
		const char *access_token_expires =
				oidc_session_get_access_token_expires(r, session);
		if (access_token_expires != NULL)
			json_object_set_new(json, OIDC_HOOK_INFO_ACCES_TOKEN_EXP,
					json_string(access_token_expires));
	}

	/* include the id_token claims in the session info */
	if (apr_hash_get(c->info_hook_data, OIDC_HOOK_INFO_ID_TOKEN,
			APR_HASH_KEY_STRING)) {
		json_t *id_token = oidc_session_get_idtoken_claims_json(r, session);
		if (id_token)
			json_object_set_new(json, OIDC_HOOK_INFO_ID_TOKEN, id_token);
	}

	if (apr_hash_get(c->info_hook_data, OIDC_HOOK_INFO_USER_INFO,
			APR_HASH_KEY_STRING)) {
		/* include the claims from the userinfo endpoint the session info */
		json_t *claims = oidc_session_get_userinfo_claims_json(r, session);
		if (claims)
			json_object_set_new(json, OIDC_HOOK_INFO_USER_INFO, claims);
	}

	if (apr_hash_get(c->info_hook_data, OIDC_HOOK_INFO_SESSION,
			APR_HASH_KEY_STRING)) {
		json_t *j_session = json_object();
		json_object_set(j_session, OIDC_HOOK_INFO_SESSION_STATE,
				session->state);
		json_object_set_new(j_session, OIDC_HOOK_INFO_SESSION_UUID,
				json_string(session->uuid));
		json_object_set_new(j_session, OIDC_HOOK_INFO_SESSION_TIMEOUT,
				json_integer(apr_time_sec(session->expiry)));
		apr_time_t session_expires = oidc_session_get_session_expires(r,
				session);
		json_object_set_new(j_session, OIDC_HOOK_INFO_SESSION_EXP,
				json_integer(apr_time_sec(session_expires)));
		json_object_set_new(j_session, OIDC_HOOK_INFO_SESSION_REMOTE_USER,
				json_string(session->remote_user));
		json_object_set_new(json, OIDC_HOOK_INFO_SESSION, j_session);

	}

	if (apr_hash_get(c->info_hook_data, OIDC_HOOK_INFO_REFRESH_TOKEN,
			APR_HASH_KEY_STRING)) {
		/* include the refresh token in the session info */
		const char *refresh_token = oidc_session_get_refresh_token(r, session);
		if (refresh_token != NULL)
			json_object_set_new(json, OIDC_HOOK_INFO_REFRESH_TOKEN,
					json_string(refresh_token));
	}

	if (apr_strnatcmp(OIDC_HOOK_INFO_FORMAT_JSON, s_format) == 0) {
		/* JSON-encode the result */
		r_value = oidc_util_encode_json_object(r, json, 0);
		/* return the stringified JSON result */
		rc = oidc_util_http_send(r, r_value, strlen(r_value),
				OIDC_CONTENT_TYPE_JSON, OK);
	} else if (apr_strnatcmp(OIDC_HOOK_INFO_FORMAT_HTML, s_format) == 0) {
		/* JSON-encode the result */
		r_value = oidc_util_encode_json_object(r, json, JSON_INDENT(2));
		rc = oidc_util_html_send(r, "Session Info", NULL, NULL,
				apr_psprintf(r->pool, "<pre>%s</pre>", r_value), OK);
	}

	/* free the allocated resources */
	json_decref(json);

	/* pass the tokens to the application and save the session, possibly updating the expiry */
	if (oidc_session_pass_tokens_and_save(r, c, session, needs_save) == FALSE) {
		oidc_warn(r, "error saving session");
		rc = HTTP_INTERNAL_SERVER_ERROR;
	}

	return rc;
}