mech_rpa_build_token4(struct rpa_auth_request *request, size_t *size)
{
	buffer_t *buf;
	unsigned char server_response[MD5_RESULTLEN];
	unsigned int length = sizeof(rpa_oid) +
		sizeof(server_response) + 1 +
		sizeof(request->session_key) + 1 + 1;

	buf = buffer_create_dynamic(request->pool, length + 4);

	buffer_append_c(buf, ASN1_APPLICATION);
	buffer_append_asn1_length(buf, length);
	buffer_append(buf, rpa_oid, sizeof(rpa_oid));

	/* Generate random session key */
	random_fill(request->session_key, sizeof(request->session_key));

	/* Server authentication response */
	rpa_server_response(request, server_response);
	buffer_append_c(buf, sizeof(server_response));
	buffer_append(buf, server_response, sizeof(server_response));

	buffer_append_c(buf, sizeof(request->session_key));
	buffer_append(buf, request->session_key, sizeof(request->session_key));

	/* Status, 0 - success */
	buffer_append_c(buf, 0);

	*size = buf->used;
	return buffer_free_without_data(&buf);
}