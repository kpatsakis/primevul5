static int oidc_handle_session_management_iframe_op(request_rec *r, oidc_cfg *c,
		oidc_session_t *session, const char *check_session_iframe) {
	oidc_debug(r, "enter");
	oidc_util_hdr_out_location_set(r, check_session_iframe);
	return HTTP_MOVED_TEMPORARILY;
}