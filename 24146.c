mech_rpa_auth_phase2(struct auth_request *auth_request,
		     const unsigned char *data, size_t data_size)
{
	struct rpa_auth_request *request =
		(struct rpa_auth_request *)auth_request;
	const char *error;

	if (!rpa_parse_token3(request, data, data_size, &error)) {
		e_info(auth_request->mech_event,
		       "invalid token 3: %s", error);
		auth_request_fail(auth_request);
		return;
	}

	auth_request_lookup_credentials(auth_request, "RPA",
					rpa_credentials_callback);
}