static void oidc_log_session_expires(request_rec *r, const char *msg,
		apr_time_t session_expires) {
	char buf[APR_RFC822_DATE_LEN + 1];
	apr_rfc822_date(buf, session_expires);
	oidc_debug(r, "%s: %s (in %" APR_TIME_T_FMT " secs from now)", msg, buf,
			apr_time_sec(session_expires - apr_time_now()));
}