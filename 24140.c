rpa_credentials_callback(enum passdb_result result,
			 const unsigned char *credentials, size_t size,
			 struct auth_request *auth_request)
{
	struct rpa_auth_request *request =
		(struct rpa_auth_request *)auth_request;
	const unsigned char *token4;
	size_t token4_size;

	switch (result) {
	case PASSDB_RESULT_OK:
		if (!verify_credentials(request, credentials, size))
			auth_request_fail(auth_request);
		else {
			token4 = mech_rpa_build_token4(request, &token4_size);
			auth_request_handler_reply_continue(auth_request,
							    token4,
							    token4_size);
			request->phase = 2;
		}
		break;
	case PASSDB_RESULT_INTERNAL_FAILURE:
		auth_request_internal_failure(auth_request);
		break;
	default:
		auth_request_fail(auth_request);
		break;
	}
}