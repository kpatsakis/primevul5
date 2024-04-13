static bool verify_credentials(struct rpa_auth_request *request,
			       const unsigned char *credentials, size_t size)
{
	unsigned char response[MD5_RESULTLEN];

	if (size != sizeof(request->pwd_md5)) {
                e_error(request->auth_request.mech_event,
			"invalid credentials length");
		return FALSE;
	}

	memcpy(request->pwd_md5, credentials, sizeof(request->pwd_md5));
	rpa_user_response(request, response);
	return mem_equals_timing_safe(response, request->user_response, sizeof(response));
}