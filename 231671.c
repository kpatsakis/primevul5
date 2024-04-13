static int oidc_clean_expired_state_cookies(request_rec *r, oidc_cfg *c,
		const char *currentCookieName, int delete_oldest) {
	int number_of_valid_state_cookies = 0;
	oidc_state_cookies_t *first = NULL, *last = NULL;
	char *cookie, *tokenizerCtx = NULL;
	char *cookies = apr_pstrdup(r->pool, oidc_util_hdr_in_cookie_get(r));
	if (cookies != NULL) {
		cookie = apr_strtok(cookies, OIDC_STR_SEMI_COLON, &tokenizerCtx);
		while (cookie != NULL) {
			while (*cookie == OIDC_CHAR_SPACE)
				cookie++;
			if (strstr(cookie, OIDC_STATE_COOKIE_PREFIX) == cookie) {
				char *cookieName = cookie;
				while (cookie != NULL && *cookie != OIDC_CHAR_EQUAL)
					cookie++;
				if (*cookie == OIDC_CHAR_EQUAL) {
					*cookie = '\0';
					cookie++;
					if ((currentCookieName == NULL)
							|| (apr_strnatcmp(cookieName, currentCookieName)
									!= 0)) {
						oidc_proto_state_t *proto_state =
								oidc_proto_state_from_cookie(r, c, cookie);
						if (proto_state != NULL) {
							json_int_t ts = oidc_proto_state_get_timestamp(
									proto_state);
							if (apr_time_now() > ts + apr_time_from_sec(c->state_timeout)) {
								oidc_error(r,
										"state (%s) has expired (original_url=%s)",
										cookieName,
										oidc_proto_state_get_original_url(
												proto_state));
								oidc_util_set_cookie(r, cookieName, "", 0,
										NULL);
							} else {
								if (first == NULL) {
									first = apr_pcalloc(r->pool,
											sizeof(oidc_state_cookies_t));
									last = first;
								} else {
									last->next = apr_pcalloc(r->pool,
											sizeof(oidc_state_cookies_t));
									last = last->next;
								}
								last->name = cookieName;
								last->timestamp = ts;
								last->next = NULL;
								number_of_valid_state_cookies++;
							}
							oidc_proto_state_destroy(proto_state);
						}
					}
				}
			}
			cookie = apr_strtok(NULL, OIDC_STR_SEMI_COLON, &tokenizerCtx);
		}
	}

	if (delete_oldest > 0)
		number_of_valid_state_cookies = oidc_delete_oldest_state_cookies(r,
				number_of_valid_state_cookies, c->max_number_of_state_cookies,
				first);

	return number_of_valid_state_cookies;
}