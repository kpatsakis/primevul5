static int oidc_handle_redirect_authorization_response(request_rec *r,
		oidc_cfg *c, oidc_session_t *session) {

	oidc_debug(r, "enter");

	/* read the parameters from the query string */
	apr_table_t *params = apr_table_make(r->pool, 8);
	oidc_util_read_form_encoded_params(r, params, r->args);

	/* do the actual work */
	return oidc_handle_authorization_response(r, c, session, params,
			OIDC_PROTO_RESPONSE_MODE_QUERY);
}