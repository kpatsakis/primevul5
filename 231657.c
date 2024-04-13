int oidc_content_handler(request_rec *r) {
	if (oidc_enabled(r) == FALSE)
		return DECLINED;
	oidc_cfg *c = ap_get_module_config(r->server->module_config,
			&auth_openidc_module);
	return oidc_util_request_matches_url(r, oidc_get_redirect_uri(r, c)) ?
			OK : DECLINED;
}