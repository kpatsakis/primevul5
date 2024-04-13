static apr_byte_t oidc_is_discovery_response(request_rec *r, oidc_cfg *cfg) {
	/*
	 * prereq: this is a call to the configured redirect_uri, now see if:
	 * the OIDC_DISC_OP_PARAM is present
	 */
	return oidc_util_request_has_parameter(r, OIDC_DISC_OP_PARAM)
			|| oidc_util_request_has_parameter(r, OIDC_DISC_USER_PARAM);
}