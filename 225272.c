static int oidc_handle_unauthorized_user22(request_rec *r) {

	oidc_cfg *c = ap_get_module_config(r->server->module_config,
			&auth_openidc_module);

	if (apr_strnatcasecmp((const char *) ap_auth_type(r), OIDC_AUTH_TYPE_OPENID_OAUTH20) == 0) {
		oidc_oauth_return_www_authenticate(r, "insufficient_scope", "Different scope(s) or other claims required");
		return HTTP_UNAUTHORIZED;
	}

	/* see if we've configured OIDCUnAutzAction for this path */
	switch (oidc_dir_cfg_unautz_action(r)) {
	case OIDC_UNAUTZ_RETURN403:
		return HTTP_FORBIDDEN;
	case OIDC_UNAUTZ_RETURN401:
		return HTTP_UNAUTHORIZED;
	case OIDC_UNAUTZ_AUTHENTICATE:
		/*
		 * exception handling: if this looks like a XMLHttpRequest call we
		 * won't redirect the user and thus avoid creating a state cookie
		 * for a non-browser (= Javascript) call that will never return from the OP
		 */
		if (oidc_is_xml_http_request(r) == TRUE)
			return HTTP_UNAUTHORIZED;
	}

	return oidc_authenticate_user(r, c, NULL, oidc_get_current_url(r), NULL,
			NULL, NULL, oidc_dir_cfg_path_auth_request_params(r), oidc_dir_cfg_path_scope(r));
}