mech_rpa_auth_phase1(struct auth_request *auth_request,
		     const unsigned char *data, size_t data_size)
{
	struct rpa_auth_request *request =
		(struct rpa_auth_request *)auth_request;
	const unsigned char *token2;
	size_t token2_size;
	const char *service, *error;

	if (!rpa_parse_token1(data, data_size, &error)) {
		e_info(auth_request->mech_event,
		       "invalid token 1: %s", error);
		auth_request_fail(auth_request);
		return;
	}

	service = t_str_lcase(auth_request->service);

	token2 = mech_rpa_build_token2(request, &token2_size);

	request->service_ucs2be = ucs2be_str(request->pool, service,
					     &request->service_len);

	auth_request_handler_reply_continue(auth_request, token2, token2_size);
	request->phase = 1;
}