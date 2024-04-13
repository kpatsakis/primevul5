static int oidc_target_link_uri_matches_configuration(request_rec *r,
		oidc_cfg *cfg, const char *target_link_uri) {

	apr_uri_t o_uri;
	apr_uri_parse(r->pool, target_link_uri, &o_uri);
	if (o_uri.hostname == NULL) {
		oidc_error(r,
				"could not parse the \"target_link_uri\" (%s) in to a valid URL: aborting.",
				target_link_uri);
		return FALSE;
	}

	apr_uri_t r_uri;
	apr_uri_parse(r->pool, oidc_get_redirect_uri(r, cfg), &r_uri);

	if (cfg->cookie_domain == NULL) {
		/* cookie_domain set: see if the target_link_uri matches the redirect_uri host (because the session cookie will be set host-wide) */
		if (apr_strnatcmp(o_uri.hostname, r_uri.hostname) != 0) {
			char *p = strstr(o_uri.hostname, r_uri.hostname);
			if ((p == NULL) || (apr_strnatcmp(r_uri.hostname, p) != 0)) {
				oidc_error(r,
						"the URL hostname (%s) of the configured " OIDCRedirectURI " does not match the URL hostname of the \"target_link_uri\" (%s): aborting to prevent an open redirect.",
						r_uri.hostname, o_uri.hostname);
				return FALSE;
			}
		}
	} else {
		/* cookie_domain set: see if the target_link_uri is within the cookie_domain */
		char *p = strstr(o_uri.hostname, cfg->cookie_domain);
		if ((p == NULL) || (apr_strnatcmp(cfg->cookie_domain, p) != 0)) {
			oidc_error(r,
					"the domain (%s) configured in " OIDCCookieDomain " does not match the URL hostname (%s) of the \"target_link_uri\" (%s): aborting to prevent an open redirect.",
					cfg->cookie_domain, o_uri.hostname, target_link_uri);
			return FALSE;
		}
	}

	/* see if the cookie_path setting matches the target_link_uri path */
	char *cookie_path = oidc_cfg_dir_cookie_path(r);
	if (cookie_path != NULL) {
		char *p = (o_uri.path != NULL) ? strstr(o_uri.path, cookie_path) : NULL;
		if ((p == NULL) || (p != o_uri.path)) {
			oidc_error(r,
					"the path (%s) configured in " OIDCCookiePath " does not match the URL path (%s) of the \"target_link_uri\" (%s): aborting to prevent an open redirect.",
					cfg->cookie_domain, o_uri.path, target_link_uri);
			return FALSE;
		} else if (strlen(o_uri.path) > strlen(cookie_path)) {
			int n = strlen(cookie_path);
			if (cookie_path[n - 1] == OIDC_CHAR_FORWARD_SLASH)
				n--;
			if (o_uri.path[n] != OIDC_CHAR_FORWARD_SLASH) {
				oidc_error(r,
						"the path (%s) configured in " OIDCCookiePath " does not match the URL path (%s) of the \"target_link_uri\" (%s): aborting to prevent an open redirect.",
						cfg->cookie_domain, o_uri.path, target_link_uri);
				return FALSE;
			}
		}
	}
	return TRUE;
}