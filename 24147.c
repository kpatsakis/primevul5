mech_rpa_auth_continue(struct auth_request *auth_request,
		       const unsigned char *data, size_t data_size)
{
	struct rpa_auth_request *request =
		(struct rpa_auth_request *)auth_request;

	switch (request->phase) {
	case 0:
		mech_rpa_auth_phase1(auth_request, data, data_size);
		break;
	case 1:
		mech_rpa_auth_phase2(auth_request, data, data_size);
		break;
	case 2:
		mech_rpa_auth_phase3(auth_request, data, data_size);
		break;
	default:
		auth_request_fail(auth_request);
		break;
	}
}