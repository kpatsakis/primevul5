int oidc_handle_remove_at_cache(request_rec *r, oidc_cfg *c) {
	char *access_token = NULL;
	oidc_util_get_request_parameter(r,
			OIDC_REDIRECT_URI_REQUEST_REMOVE_AT_CACHE, &access_token);

	char *cache_entry = NULL;
	oidc_cache_get_access_token(r, access_token, &cache_entry);
	if (cache_entry == NULL) {
		oidc_error(r, "no cached access token found for value: %s",
				access_token);
		return HTTP_NOT_FOUND;
	}

	oidc_cache_set_access_token(r, access_token, NULL, 0);

	return OK;
}