static int oidc_authorization_request_set_cookie(request_rec *r, oidc_cfg *c,
		const char *state, oidc_proto_state_t *proto_state) {
	/*
	 * create a cookie consisting of 8 elements:
	 * random value, original URL, original method, issuer, response_type, response_mod, prompt and timestamp
	 * encoded as JSON, encrypting the resulting JSON value
	 */
	char *cookieValue = oidc_proto_state_to_cookie(r, c, proto_state);
	if (cookieValue == NULL)
		return HTTP_INTERNAL_SERVER_ERROR;

	/*
	 * clean expired state cookies to avoid pollution and optionally
	 * try to avoid the number of state cookies exceeding a max
	 */
	int number_of_cookies = oidc_clean_expired_state_cookies(r, c, NULL,
			oidc_cfg_delete_oldest_state_cookies(c));
	int max_number_of_cookies = oidc_cfg_max_number_of_state_cookies(c);
	if ((max_number_of_cookies > 0)
			&& (number_of_cookies >= max_number_of_cookies)) {

		oidc_warn(r,
				"the number of existing, valid state cookies (%d) has exceeded the limit (%d), no additional authorization request + state cookie can be generated, aborting the request",
				number_of_cookies, max_number_of_cookies);
		/*
		 * TODO: the html_send code below caters for the case that there's a user behind a
		 * browser generating this request, rather than a piece of XHR code; how would an
		 * XHR client handle this?
		 */

		/*
		 * it appears that sending content with a 503 turns the HTTP status code
		 * into a 200 so we'll avoid that for now: the user will see Apache specific
		 * readable text anyway
		 *
		return oidc_util_html_send_error(r, c->error_template,
				"Too Many Outstanding Requests",
				apr_psprintf(r->pool,
						"No authentication request could be generated since there are too many outstanding authentication requests already; you may have to wait up to %d seconds to be able to create a new request",
						c->state_timeout),
						HTTP_SERVICE_UNAVAILABLE);
		*/

		return HTTP_SERVICE_UNAVAILABLE;
	}

	/* assemble the cookie name for the state cookie */
	const char *cookieName = oidc_get_state_cookie_name(r, state);

	/* set it as a cookie */
	oidc_util_set_cookie(r, cookieName, cookieValue, -1,
			c->cookie_same_site ? OIDC_COOKIE_EXT_SAME_SITE_LAX : NULL);

	return HTTP_OK;
}