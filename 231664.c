static char *oidc_make_sid_iss_unique(request_rec *r, const char *sid,
		const char *issuer) {
	return apr_psprintf(r->pool, "%s@%s", sid, issuer);
}