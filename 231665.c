static const char *oidc_original_request_method(request_rec *r, oidc_cfg *cfg,
		apr_byte_t handle_discovery_response) {
	const char *method = OIDC_METHOD_GET;

	char *m = NULL;
	if ((handle_discovery_response == TRUE)
			&& (oidc_util_request_matches_url(r, oidc_get_redirect_uri(r, cfg)))
			&& (oidc_is_discovery_response(r, cfg))) {
		oidc_util_get_request_parameter(r, OIDC_DISC_RM_PARAM, &m);
		if (m != NULL)
			method = apr_pstrdup(r->pool, m);
	} else {

		/*
		 * if POST preserve is not enabled for this location, there's no point in preserving
		 * the method either which would result in POSTing empty data on return;
		 * so we revert to legacy behavior
		 */
		if (oidc_cfg_dir_preserve_post(r) == 0)
			return OIDC_METHOD_GET;

		const char *content_type = oidc_util_hdr_in_content_type_get(r);
		if ((r->method_number == M_POST) && (apr_strnatcmp(content_type,
				OIDC_CONTENT_TYPE_FORM_ENCODED) == 0))
			method = OIDC_METHOD_FORM_POST;
	}

	oidc_debug(r, "return: %s", method);

	return method;
}