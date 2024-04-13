static char *oidc_get_browser_state_hash(request_rec *r, const char *nonce) {

	oidc_debug(r, "enter");

	/* helper to hold to header values */
	const char *value = NULL;
	/* the hash context */
	apr_sha1_ctx_t sha1;

	/* Initialize the hash context */
	apr_sha1_init(&sha1);

	/* get the X-FORWARDED-FOR header value  */
	value = oidc_util_hdr_in_x_forwarded_for_get(r);
	/* if we have a value for this header, concat it to the hash input */
	if (value != NULL)
		apr_sha1_update(&sha1, value, strlen(value));

	/* get the USER-AGENT header value  */
	value = oidc_util_hdr_in_user_agent_get(r);
	/* if we have a value for this header, concat it to the hash input */
	if (value != NULL)
		apr_sha1_update(&sha1, value, strlen(value));

	/* get the remote client IP address or host name */
	/*
	int remotehost_is_ip;
	value = ap_get_remote_host(r->connection, r->per_dir_config,
			REMOTE_NOLOOKUP, &remotehost_is_ip);
	apr_sha1_update(&sha1, value, strlen(value));
	*/

	/* concat the nonce parameter to the hash input */
	apr_sha1_update(&sha1, nonce, strlen(nonce));

	/* concat the token binding ID if present */
	value = oidc_util_get_provided_token_binding_id(r);
	if (value != NULL) {
		oidc_debug(r,
				"Provided Token Binding ID environment variable found; adding its value to the state");
		apr_sha1_update(&sha1, value, strlen(value));
	}

	/* finalize the hash input and calculate the resulting hash output */
	unsigned char hash[OIDC_SHA1_LEN];
	apr_sha1_final(hash, &sha1);

	/* base64url-encode the resulting hash and return it */
	char *result = NULL;
	oidc_base64url_encode(r, &result, (const char *) hash, OIDC_SHA1_LEN, TRUE);
	return result;
}