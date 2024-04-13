void oidc_scrub_headers(request_rec *r) {
	oidc_cfg *cfg = ap_get_module_config(r->server->module_config,
			&auth_openidc_module);

	const char *prefix = oidc_cfg_claim_prefix(r);
	apr_hash_t *hdrs = apr_hash_make(r->pool);

	if (apr_strnatcmp(prefix, "") == 0) {
		if ((cfg->white_listed_claims != NULL)
				&& (apr_hash_count(cfg->white_listed_claims) > 0))
			hdrs = apr_hash_overlay(r->pool, cfg->white_listed_claims, hdrs);
		else
			oidc_warn(r,
					"both " OIDCClaimPrefix " and " OIDCWhiteListedClaims " are empty: this renders an insecure setup!");
	}

	char *authn_hdr = oidc_cfg_dir_authn_header(r);
	if (authn_hdr != NULL)
		apr_hash_set(hdrs, authn_hdr, APR_HASH_KEY_STRING, authn_hdr);

	/*
	 * scrub all headers starting with OIDC_ first
	 */
	oidc_scrub_request_headers(r, OIDC_DEFAULT_HEADER_PREFIX, hdrs);

	/*
	 * then see if the claim headers need to be removed on top of that
	 * (i.e. the prefix does not start with the default OIDC_)
	 */
	if ((strstr(prefix, OIDC_DEFAULT_HEADER_PREFIX) != prefix)) {
		oidc_scrub_request_headers(r, prefix, NULL);
	}
}