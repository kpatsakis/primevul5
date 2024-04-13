static int oidc_handle_request_uri(request_rec *r, oidc_cfg *c) {

	char *request_ref = NULL;
	oidc_util_get_request_parameter(r, OIDC_REDIRECT_URI_REQUEST_REQUEST_URI,
			&request_ref);
	if (request_ref == NULL) {
		oidc_error(r, "no \"%s\" parameter found",
				OIDC_REDIRECT_URI_REQUEST_REQUEST_URI);
		return HTTP_BAD_REQUEST;
	}

	char *jwt = NULL;
	oidc_cache_get_request_uri(r, request_ref, &jwt);
	if (jwt == NULL) {
		oidc_error(r, "no cached JWT found for %s reference: %s",
				OIDC_REDIRECT_URI_REQUEST_REQUEST_URI, request_ref);
		return HTTP_NOT_FOUND;
	}

	oidc_cache_set_request_uri(r, request_ref, NULL, 0);

	return oidc_util_http_send(r, jwt, strlen(jwt), OIDC_CONTENT_TYPE_JWT, OK);
}