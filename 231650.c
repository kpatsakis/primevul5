static authz_status oidc_handle_unauthorized_user24(request_rec *r) {

	oidc_debug(r, "enter");

	oidc_cfg *c = ap_get_module_config(r->server->module_config,
			&auth_openidc_module);

	if (apr_strnatcasecmp((const char *) ap_auth_type(r),
			OIDC_AUTH_TYPE_OPENID_OAUTH20) == 0) {
		oidc_oauth_return_www_authenticate(r, "insufficient_scope",
				"Different scope(s) or other claims required");
		return AUTHZ_DENIED;
	}

	/* see if we've configured OIDCUnAutzAction for this path */
	switch (oidc_dir_cfg_unautz_action(r)) {
	// TODO: document that AuthzSendForbiddenOnFailure is required to return 403 FORBIDDEN
	case OIDC_UNAUTZ_RETURN403:
	case OIDC_UNAUTZ_RETURN401:
		return AUTHZ_DENIED;
		break;
	case OIDC_UNAUTZ_AUTHENTICATE:
		/*
		 * exception handling: if this looks like a XMLHttpRequest call we
		 * won't redirect the user and thus avoid creating a state cookie
		 * for a non-browser (= Javascript) call that will never return from the OP
		 */
		if (oidc_is_xml_http_request(r) == TRUE)
			return AUTHZ_DENIED;
		break;
	}

	oidc_authenticate_user(r, c, NULL, oidc_get_current_url(r), NULL,
			NULL, NULL, oidc_dir_cfg_path_auth_request_params(r),
			oidc_dir_cfg_path_scope(r));

	const char *location = oidc_util_hdr_out_location_get(r);
	if (location != NULL) {
		oidc_debug(r, "send HTML refresh with authorization redirect: %s",
				location);

		char *html_head = apr_psprintf(r->pool,
				"<meta http-equiv=\"refresh\" content=\"0; url=%s\">",
				location);
		oidc_util_html_send(r, "Stepup Authentication", html_head, NULL, NULL,
				HTTP_UNAUTHORIZED);
	}

	return AUTHZ_DENIED;
}