static apr_byte_t oidc_set_request_user(request_rec *r, oidc_cfg *c,
		oidc_provider_t *provider, oidc_jwt_t *jwt, const char *s_claims) {

	char *issuer = provider->issuer;
	char *claim_name = apr_pstrdup(r->pool, c->remote_user_claim.claim_name);
	int n = strlen(claim_name);
	apr_byte_t post_fix_with_issuer = (claim_name[n - 1] == OIDC_CHAR_AT);
	if (post_fix_with_issuer == TRUE) {
		claim_name[n - 1] = '\0';
		issuer =
				(strstr(issuer, "https://") == NULL) ?
						apr_pstrdup(r->pool, issuer) :
						apr_pstrdup(r->pool, issuer + strlen("https://"));
	}

	/* extract the username claim (default: "sub") from the id_token payload or user claims */
	apr_byte_t rc = FALSE;
	char *remote_user = NULL;
	json_t *claims = NULL;
	oidc_util_decode_json_object(r, s_claims, &claims);
	if (claims == NULL) {
		rc = oidc_get_remote_user(r, claim_name, c->remote_user_claim.reg_exp,
				c->remote_user_claim.replace, jwt->payload.value.json,
				&remote_user);
	} else {
		oidc_util_json_merge(r, jwt->payload.value.json, claims);
		rc = oidc_get_remote_user(r, claim_name, c->remote_user_claim.reg_exp,
				c->remote_user_claim.replace, claims, &remote_user);
		json_decref(claims);
	}

	if ((rc == FALSE) || (remote_user == NULL)) {
		oidc_error(r,
				"" OIDCRemoteUserClaim "is set to \"%s\", but could not set the remote user based on the requested claim \"%s\" and the available claims for the user",
				c->remote_user_claim.claim_name, claim_name);
		return FALSE;
	}

	if (post_fix_with_issuer == TRUE)
		remote_user = apr_psprintf(r->pool, "%s%s%s", remote_user, OIDC_STR_AT,
				issuer);

	r->user = apr_pstrdup(r->pool, remote_user);

	oidc_debug(r, "set remote_user to \"%s\" based on claim: \"%s\"%s", r->user,
			c->remote_user_claim.claim_name,
			c->remote_user_claim.reg_exp ?
					apr_psprintf(r->pool,
							" and expression: \"%s\" and replace string: \"%s\"",
							c->remote_user_claim.reg_exp,
							c->remote_user_claim.replace) :
							"");

	return TRUE;
}