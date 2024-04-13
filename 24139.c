mech_rpa_auth_phase3(struct auth_request *auth_request,
		     const unsigned char *data, size_t data_size)
{
	static const unsigned char client_ack[3] = { 0x60, 0x01, 0x00 };

	if ((data_size != sizeof(client_ack)) ||
	    (memcmp(data, client_ack, sizeof(client_ack)) != 0)) {
		e_info(auth_request->mech_event,
		       "invalid token 5 or client rejects us");
		auth_request_fail(auth_request);
	} else {
		auth_request_success(auth_request, "", 0);
	}
}