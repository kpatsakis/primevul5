static void oidc_authz_get_claims_and_idtoken(request_rec *r, json_t **claims,
		json_t **id_token) {

	const char *s_claims = oidc_request_state_get(r,
			OIDC_REQUEST_STATE_KEY_CLAIMS);
	if (s_claims != NULL)
		oidc_util_decode_json_object(r, s_claims, claims);

	const char *s_id_token = oidc_request_state_get(r,
			OIDC_REQUEST_STATE_KEY_IDTOKEN);
	if (s_id_token != NULL)
		oidc_util_decode_json_object(r, s_id_token, id_token);
}