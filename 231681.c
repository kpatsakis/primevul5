void oidc_strip_cookies(request_rec *r) {

	char *cookie, *ctx, *result = NULL;
	const char *name = NULL;
	int i;

	apr_array_header_t *strip = oidc_dir_cfg_strip_cookies(r);

	char *cookies = apr_pstrdup(r->pool, oidc_util_hdr_in_cookie_get(r));

	if ((cookies != NULL) && (strip != NULL)) {

		oidc_debug(r,
				"looking for the following cookies to strip from cookie header: %s",
				apr_array_pstrcat(r->pool, strip, OIDC_CHAR_COMMA));

		cookie = apr_strtok(cookies, OIDC_STR_SEMI_COLON, &ctx);

		do {
			while (cookie != NULL && *cookie == OIDC_CHAR_SPACE)
				cookie++;

			for (i = 0; i < strip->nelts; i++) {
				name = ((const char**) strip->elts)[i];
				if ((strncmp(cookie, name, strlen(name)) == 0)
						&& (cookie[strlen(name)] == OIDC_CHAR_EQUAL)) {
					oidc_debug(r, "stripping: %s", name);
					break;
				}
			}

			if (i == strip->nelts) {
				result = result ? apr_psprintf(r->pool, "%s%s%s", result,
						OIDC_STR_SEMI_COLON, cookie) :
						cookie;
			}

			cookie = apr_strtok(NULL, OIDC_STR_SEMI_COLON, &ctx);
		} while (cookie != NULL);

		oidc_util_hdr_in_cookie_set(r, result);
	}
}