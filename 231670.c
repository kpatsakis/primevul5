int oidc_check_user_id(request_rec *r) {

	oidc_cfg *c = ap_get_module_config(r->server->module_config,
			&auth_openidc_module);

	/* log some stuff about the incoming HTTP request */
	oidc_debug(r, "incoming request: \"%s?%s\", ap_is_initial_req(r)=%d",
			r->parsed_uri.path, r->args, ap_is_initial_req(r));

	/* see if any authentication has been defined at all */
	if (ap_auth_type(r) == NULL)
		return DECLINED;

	/* see if we've configured OpenID Connect user authentication for this request */
	if (apr_strnatcasecmp((const char *) ap_auth_type(r),
			OIDC_AUTH_TYPE_OPENID_CONNECT) == 0)
		return oidc_check_userid_openidc(r, c);

	/* see if we've configured OAuth 2.0 access control for this request */
	if (apr_strnatcasecmp((const char *) ap_auth_type(r),
			OIDC_AUTH_TYPE_OPENID_OAUTH20) == 0)
		return oidc_oauth_check_userid(r, c, NULL);

	/* see if we've configured "mixed mode" for this request */
	if (apr_strnatcasecmp((const char *) ap_auth_type(r),
			OIDC_AUTH_TYPE_OPENID_BOTH) == 0)
		return oidc_check_mixed_userid_oauth(r, c);

	/* this is not for us but for some other handler */
	return DECLINED;
}