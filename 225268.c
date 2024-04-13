static int oidc_check_mixed_userid_oauth(request_rec *r, oidc_cfg *c) {

	/* get the bearer access token from the Authorization header */
	const char *access_token = NULL;
	if (oidc_oauth_get_bearer_token(r, &access_token) == TRUE) {

		r->ap_auth_type = apr_pstrdup(r->pool, OIDC_AUTH_TYPE_OPENID_OAUTH20);
		return oidc_oauth_check_userid(r, c, access_token);
	}

	/* no bearer token found: then treat this as a regular OIDC browser request */
	r->ap_auth_type = apr_pstrdup(r->pool, OIDC_AUTH_TYPE_OPENID_CONNECT);
	return oidc_check_userid_openidc(r, c);
}