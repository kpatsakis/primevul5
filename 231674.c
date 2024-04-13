static apr_byte_t oidc_unsolicited_proto_state(request_rec *r, oidc_cfg *c,
		const char *state, oidc_proto_state_t **proto_state) {

	char *alg = NULL;
	oidc_debug(r, "enter: state header=%s",
			oidc_proto_peek_jwt_header(r, state, &alg));

	oidc_jose_error_t err;
	oidc_jwk_t *jwk = NULL;
	if (oidc_util_create_symmetric_key(r, c->provider.client_secret,
			oidc_alg2keysize(alg), OIDC_JOSE_ALG_SHA256,
			TRUE, &jwk) == FALSE)
		return FALSE;

	oidc_jwt_t *jwt = NULL;
	if (oidc_jwt_parse(r->pool, state, &jwt,
			oidc_util_merge_symmetric_key(r->pool, c->private_keys, jwk),
			&err) == FALSE) {
		oidc_error(r,
				"could not parse JWT from state: invalid unsolicited response: %s",
				oidc_jose_e2s(r->pool, err));
		return FALSE;
	}

	oidc_jwk_destroy(jwk);
	oidc_debug(r, "successfully parsed JWT from state");

	if (jwt->payload.iss == NULL) {
		oidc_error(r, "no \"%s\" could be retrieved from JWT state, aborting",
				OIDC_CLAIM_ISS);
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	oidc_provider_t *provider = oidc_get_provider_for_issuer(r, c,
			jwt->payload.iss, FALSE);
	if (provider == NULL) {
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	/* validate the state JWT, validating optional exp + iat */
	if (oidc_proto_validate_jwt(r, jwt, provider->issuer, FALSE, FALSE,
			provider->idtoken_iat_slack,
			OIDC_TOKEN_BINDING_POLICY_DISABLED) == FALSE) {
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	char *rfp = NULL;
	if (oidc_jose_get_string(r->pool, jwt->payload.value.json, OIDC_CLAIM_RFP,
			TRUE, &rfp, &err) == FALSE) {
		oidc_error(r,
				"no \"%s\" claim could be retrieved from JWT state, aborting: %s",
				OIDC_CLAIM_RFP, oidc_jose_e2s(r->pool, err));
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	if (apr_strnatcmp(rfp, OIDC_PROTO_ISS) != 0) {
		oidc_error(r, "\"%s\" (%s) does not match \"%s\", aborting",
				OIDC_CLAIM_RFP, rfp, OIDC_PROTO_ISS);
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	char *target_link_uri = NULL;
	oidc_jose_get_string(r->pool, jwt->payload.value.json,
			OIDC_CLAIM_TARGET_LINK_URI,
			FALSE, &target_link_uri, NULL);
	if (target_link_uri == NULL) {
		if (c->default_sso_url == NULL) {
			oidc_error(r,
					"no \"%s\" claim could be retrieved from JWT state and no " OIDCDefaultURL " is set, aborting",
					OIDC_CLAIM_TARGET_LINK_URI);
			oidc_jwt_destroy(jwt);
			return FALSE;
		}
		target_link_uri = c->default_sso_url;
	}

	if (c->metadata_dir != NULL) {
		if ((oidc_metadata_get(r, c, jwt->payload.iss, &provider, FALSE)
				== FALSE) || (provider == NULL)) {
			oidc_error(r, "no provider metadata found for provider \"%s\"",
					jwt->payload.iss);
			oidc_jwt_destroy(jwt);
			return FALSE;
		}
	}

	char *jti = NULL;
	oidc_jose_get_string(r->pool, jwt->payload.value.json, OIDC_CLAIM_JTI,
			FALSE, &jti,
			NULL);
	if (jti == NULL) {
		char *cser = oidc_jwt_serialize(r->pool, jwt, &err);
		if (cser == NULL)
			return FALSE;
		if (oidc_util_hash_string_and_base64url_encode(r, OIDC_JOSE_ALG_SHA256,
				cser, &jti) == FALSE) {
			oidc_error(r,
					"oidc_util_hash_string_and_base64url_encode returned an error");
			return FALSE;
		}
	}

	char *replay = NULL;
	oidc_cache_get_jti(r, jti, &replay);
	if (replay != NULL) {
		oidc_error(r,
				"the \"%s\" value (%s) passed in the browser state was found in the cache already; possible replay attack!?",
				OIDC_CLAIM_JTI, jti);
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	/* jti cache duration is the configured replay prevention window for token issuance plus 10 seconds for safety */
	apr_time_t jti_cache_duration = apr_time_from_sec(
			provider->idtoken_iat_slack * 2 + 10);

	/* store it in the cache for the calculated duration */
	oidc_cache_set_jti(r, jti, jti, apr_time_now() + jti_cache_duration);

	oidc_debug(r,
			"jti \"%s\" validated successfully and is now cached for %" APR_TIME_T_FMT " seconds",
			jti, apr_time_sec(jti_cache_duration));

	jwk = NULL;
	if (oidc_util_create_symmetric_key(r, c->provider.client_secret, 0,
			NULL, TRUE, &jwk) == FALSE)
		return FALSE;

	oidc_jwks_uri_t jwks_uri = { provider->jwks_uri,
			provider->jwks_refresh_interval, provider->ssl_validate_server };
	if (oidc_proto_jwt_verify(r, c, jwt, &jwks_uri,
			oidc_util_merge_symmetric_key(r->pool, NULL, jwk)) == FALSE) {
		oidc_error(r, "state JWT could not be validated, aborting");
		oidc_jwt_destroy(jwt);
		return FALSE;
	}

	oidc_jwk_destroy(jwk);
	oidc_debug(r, "successfully verified state JWT");

	*proto_state = oidc_proto_state_new();
	oidc_proto_state_set_issuer(*proto_state, jwt->payload.iss);
	oidc_proto_state_set_original_url(*proto_state, target_link_uri);
	oidc_proto_state_set_original_method(*proto_state, OIDC_METHOD_GET);
	oidc_proto_state_set_response_mode(*proto_state, provider->response_mode);
	oidc_proto_state_set_response_type(*proto_state, provider->response_type);
	oidc_proto_state_set_timestamp_now(*proto_state);

	oidc_jwt_destroy(jwt);

	return TRUE;
}