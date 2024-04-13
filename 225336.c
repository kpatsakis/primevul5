static char* oidc_get_state_cookie_name(request_rec *r, const char *state) {
	return apr_psprintf(r->pool, "%s%s", oidc_cfg_dir_state_cookie_prefix(r),
			state);
}