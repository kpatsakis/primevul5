apr_byte_t oidc_enabled(request_rec *r) {
	if (ap_auth_type(r) == NULL)
		return FALSE;

	if (apr_strnatcasecmp((const char *) ap_auth_type(r),
			OIDC_AUTH_TYPE_OPENID_CONNECT) == 0)
		return TRUE;

	if (apr_strnatcasecmp((const char *) ap_auth_type(r),
			OIDC_AUTH_TYPE_OPENID_OAUTH20) == 0)
		return TRUE;

	if (apr_strnatcasecmp((const char *) ap_auth_type(r),
			OIDC_AUTH_TYPE_OPENID_BOTH) == 0)
		return TRUE;

	return FALSE;
}