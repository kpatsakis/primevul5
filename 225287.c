apr_byte_t oidc_get_remote_user(request_rec *r, const char *claim_name,
		const char *reg_exp, const char *replace, json_t *json,
		char **request_user) {

	/* get the claim value from the JSON object */
	json_t *username = json_object_get(json, claim_name);
	if ((username == NULL) || (!json_is_string(username))) {
		oidc_warn(r, "JSON object did not contain a \"%s\" string", claim_name);
		return FALSE;
	}

	*request_user = apr_pstrdup(r->pool, json_string_value(username));

	if (reg_exp != NULL) {

		char *error_str = NULL;

		if (replace == NULL) {

			if (oidc_util_regexp_first_match(r->pool, *request_user, reg_exp,
					request_user, &error_str) == FALSE) {
				oidc_error(r, "oidc_util_regexp_first_match failed: %s",
						error_str);
				*request_user = NULL;
				return FALSE;
			}

		} else if (oidc_util_regexp_substitute(r->pool, *request_user, reg_exp,
				replace, request_user, &error_str) == FALSE) {

			oidc_error(r, "oidc_util_regexp_substitute failed: %s", error_str);
			*request_user = NULL;
			return FALSE;
		}

	}

	return TRUE;
}