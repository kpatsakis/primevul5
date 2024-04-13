static apr_byte_t oidc_is_xml_http_request(request_rec *r) {

	if ((oidc_util_hdr_in_x_requested_with_get(r) != NULL)
			&& (apr_strnatcasecmp(oidc_util_hdr_in_x_requested_with_get(r),
					OIDC_HTTP_HDR_VAL_XML_HTTP_REQUEST) == 0))
		return TRUE;

	if ((oidc_util_hdr_in_accept_contains(r, OIDC_CONTENT_TYPE_TEXT_HTML)
			== FALSE) && (oidc_util_hdr_in_accept_contains(r,
					OIDC_CONTENT_TYPE_APP_XHTML_XML) == FALSE)
					&& (oidc_util_hdr_in_accept_contains(r,
							OIDC_CONTENT_TYPE_ANY) == FALSE))
		return TRUE;

	return FALSE;
}