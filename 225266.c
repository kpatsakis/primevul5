static int oidc_authorization_response_error(request_rec *r, oidc_cfg *c,
		oidc_proto_state_t *proto_state, const char *error,
		const char *error_description) {
	const char *prompt = oidc_proto_state_get_prompt(proto_state);
	if (prompt != NULL)
		prompt = apr_pstrdup(r->pool, prompt);
	oidc_proto_state_destroy(proto_state);
	if ((prompt != NULL)
			&& (apr_strnatcmp(prompt, OIDC_PROTO_PROMPT_NONE) == 0)) {
		return oidc_session_redirect_parent_window_to_logout(r, c);
	}
	return oidc_util_html_send_error(r, c->error_template,
			apr_psprintf(r->pool, "OpenID Connect Provider error: %s", error),
			error_description, OK);
}