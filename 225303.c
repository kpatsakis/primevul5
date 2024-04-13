static apr_byte_t oidc_handle_flows(request_rec *r, oidc_cfg *c,
		oidc_proto_state_t *proto_state, oidc_provider_t *provider,
		apr_table_t *params, const char *response_mode, oidc_jwt_t **jwt) {

	apr_byte_t rc = FALSE;

	const char *requested_response_type = oidc_proto_state_get_response_type(
			proto_state);

	/* handle the requested response type/mode */
	if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			OIDC_PROTO_RESPONSE_TYPE_CODE_IDTOKEN_TOKEN)) {
		rc = oidc_proto_authorization_response_code_idtoken_token(r, c,
				proto_state, provider, params, response_mode, jwt);
	} else if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			OIDC_PROTO_RESPONSE_TYPE_CODE_IDTOKEN)) {
		rc = oidc_proto_authorization_response_code_idtoken(r, c, proto_state,
				provider, params, response_mode, jwt);
	} else if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			OIDC_PROTO_RESPONSE_TYPE_CODE_TOKEN)) {
		rc = oidc_proto_handle_authorization_response_code_token(r, c,
				proto_state, provider, params, response_mode, jwt);
	} else if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			OIDC_PROTO_RESPONSE_TYPE_CODE)) {
		rc = oidc_proto_handle_authorization_response_code(r, c, proto_state,
				provider, params, response_mode, jwt);
	} else if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			OIDC_PROTO_RESPONSE_TYPE_IDTOKEN_TOKEN)) {
		rc = oidc_proto_handle_authorization_response_idtoken_token(r, c,
				proto_state, provider, params, response_mode, jwt);
	} else if (oidc_util_spaced_string_equals(r->pool, requested_response_type,
			OIDC_PROTO_RESPONSE_TYPE_IDTOKEN)) {
		rc = oidc_proto_handle_authorization_response_idtoken(r, c, proto_state,
				provider, params, response_mode, jwt);
	} else {
		oidc_error(r, "unsupported response type: \"%s\"",
				requested_response_type);
	}

	if ((rc == FALSE) && (*jwt != NULL)) {
		oidc_jwt_destroy(*jwt);
		*jwt = NULL;
	}

	return rc;
}