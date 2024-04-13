int oidc_handle_jwks(request_rec *r, oidc_cfg *c) {

	/* pickup requested JWKs type */
	//	char *jwks_type = NULL;
	//	oidc_util_get_request_parameter(r, OIDC_REDIRECT_URI_REQUEST_JWKS, &jwks_type);
	char *jwks = apr_pstrdup(r->pool, "{ \"keys\" : [");
	int i = 0;
	apr_byte_t first = TRUE;
	oidc_jose_error_t err;

	if (c->public_keys != NULL) {

		/* loop over the RSA public keys */
		for (i = 0; i < c->public_keys->nelts; i++) {
			const oidc_jwk_t *jwk =
					((const oidc_jwk_t**) c->public_keys->elts)[i];
			char *s_json = NULL;

			if (oidc_jwk_to_json(r->pool, jwk, &s_json, &err) == TRUE) {
				jwks = apr_psprintf(r->pool, "%s%s %s ", jwks, first ? "" : ",",
						s_json);
				first = FALSE;
			} else {
				oidc_error(r,
						"could not convert RSA JWK to JSON using oidc_jwk_to_json: %s",
						oidc_jose_e2s(r->pool, err));
			}
		}
	}

	// TODO: send stuff if first == FALSE?
	jwks = apr_psprintf(r->pool, "%s ] }", jwks);

	return oidc_util_http_send(r, jwks, strlen(jwks), OIDC_CONTENT_TYPE_JSON,
			OK);
}