static int oidc_handle_unauthenticated_user(request_rec *r, oidc_cfg *c) {

	/* see if we've configured OIDCUnAuthAction for this path */
	switch (oidc_dir_cfg_unauth_action(r)) {
	case OIDC_UNAUTH_RETURN410:
		return HTTP_GONE;
	case OIDC_UNAUTH_RETURN401:
		return HTTP_UNAUTHORIZED;
	case OIDC_UNAUTH_PASS:
		r->user = "";

		/*
		 * we're not going to pass information about an authenticated user to the application,
		 * but we do need to scrub the headers that mod_auth_openidc would set for security reasons
		 */
		oidc_scrub_headers(r);

		return OK;

	case OIDC_UNAUTH_AUTHENTICATE:

		/*
		 * exception handling: if this looks like a XMLHttpRequest call we
		 * won't redirect the user and thus avoid creating a state cookie
		 * for a non-browser (= Javascript) call that will never return from the OP
		 */
		if (oidc_is_xml_http_request(r) == TRUE)
			return HTTP_UNAUTHORIZED;
	}

	/*
	 * else: no session (regardless of whether it is main or sub-request),
	 * and we need to authenticate the user
	 */
	return oidc_authenticate_user(r, c, NULL, oidc_get_current_url(r), NULL,
			NULL, NULL, oidc_dir_cfg_path_auth_request_params(r),
			oidc_dir_cfg_path_scope(r));
}