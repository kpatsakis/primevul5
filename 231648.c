static int oidc_handle_post_authorization_response(request_rec *r, oidc_cfg *c,
		oidc_session_t *session) {

	oidc_debug(r, "enter");

	/* initialize local variables */
	char *response_mode = NULL;

	/* read the parameters that are POST-ed to us */
	apr_table_t *params = apr_table_make(r->pool, 8);
	if (oidc_util_read_post_params(r, params, FALSE, NULL) == FALSE) {
		oidc_error(r, "something went wrong when reading the POST parameters");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	/* see if we've got any POST-ed data at all */
	if ((apr_table_elts(params)->nelts < 1)
			|| ((apr_table_elts(params)->nelts == 1)
					&& apr_table_get(params, OIDC_PROTO_RESPONSE_MODE)
					&& (apr_strnatcmp(
							apr_table_get(params, OIDC_PROTO_RESPONSE_MODE),
							OIDC_PROTO_RESPONSE_MODE_FRAGMENT) == 0))) {
		return oidc_util_html_send_error(r, c->error_template,
				"Invalid Request",
				"You've hit an OpenID Connect Redirect URI with no parameters, this is an invalid request; you should not open this URL in your browser directly, or have the server administrator use a different " OIDCRedirectURI " setting.",
				HTTP_INTERNAL_SERVER_ERROR);
	}

	/* get the parameters */
	response_mode = (char *) apr_table_get(params, OIDC_PROTO_RESPONSE_MODE);

	/* do the actual implicit work */
	return oidc_handle_authorization_response(r, c, session, params,
			response_mode ? response_mode : OIDC_PROTO_RESPONSE_MODE_FORM_POST);
}