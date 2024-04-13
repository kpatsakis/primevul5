static apr_byte_t oidc_provider_static_config(request_rec *r, oidc_cfg *c,
		oidc_provider_t **provider) {

	json_t *j_provider = NULL;
	char *s_json = NULL;

	/* see if we should configure a static provider based on external (cached) metadata */
	if ((c->metadata_dir != NULL) || (c->provider.metadata_url == NULL)) {
		*provider = &c->provider;
		return TRUE;
	}

	oidc_cache_get_provider(r, c->provider.metadata_url, &s_json);

	if (s_json == NULL) {

		if (oidc_metadata_provider_retrieve(r, c, NULL,
				c->provider.metadata_url, &j_provider, &s_json) == FALSE) {
			oidc_error(r, "could not retrieve metadata from url: %s",
					c->provider.metadata_url);
			return FALSE;
		}

		oidc_cache_set_provider(r, c->provider.metadata_url, s_json,
				apr_time_now() + (c->provider_metadata_refresh_interval <= 0 ? apr_time_from_sec( OIDC_CACHE_PROVIDER_METADATA_EXPIRY_DEFAULT) : c->provider_metadata_refresh_interval));

	} else {

		oidc_util_decode_json_object(r, s_json, &j_provider);

		/* check to see if it is valid metadata */
		if (oidc_metadata_provider_is_valid(r, c, j_provider, NULL) == FALSE) {
			oidc_error(r,
					"cache corruption detected: invalid metadata from url: %s",
					c->provider.metadata_url);
			return FALSE;
		}
	}

	*provider = apr_pcalloc(r->pool, sizeof(oidc_provider_t));
	memcpy(*provider, &c->provider, sizeof(oidc_provider_t));

	if (oidc_metadata_provider_parse(r, c, j_provider, *provider) == FALSE) {
		oidc_error(r, "could not parse metadata from url: %s",
				c->provider.metadata_url);
		if (j_provider)
			json_decref(j_provider);
		return FALSE;
	}

	json_decref(j_provider);

	return TRUE;
}