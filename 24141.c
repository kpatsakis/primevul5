rpa_parse_token3(struct rpa_auth_request *request, const void *data,
		 size_t data_size, const char **error)
{
	struct auth_request *auth_request = &request->auth_request;
	const unsigned char *end = ((const unsigned char *)data) + data_size;
	const unsigned char *p;
	unsigned int len;
	const char *user, *realm;

	p = rpa_check_message(data, end, error);
	if (p == NULL)
		return FALSE;

	/* Read username@realm */
	if (p + 2 > end) {
		*error = "message too short";
		return FALSE;
	}

	len = (p[0] << 8) + p[1];
	if (p + 2 + len > end) {
		*error = "message too short";
		return FALSE;
	}
	p += 2;

	user = t_strndup(p, len);
	realm = strrchr(user, '@');
	if (realm == NULL) {
		*error = "missing realm";
		return FALSE;
	}
	user = t_strdup_until(user, realm++);
	p += len;

	if (!auth_request_set_username(auth_request, user, error))
		return FALSE;

	request->username_ucs2be = ucs2be_str(request->pool, auth_request->user,
					      &request->username_len);
	request->realm_ucs2be = ucs2be_str(request->pool, realm,
					   &request->realm_len);

	/* Read user challenge */
	request->user_challenge_len = rpa_read_buffer(request->pool, &p, end,
						      &request->user_challenge);
	if (request->user_challenge_len == 0) {
		*error = "invalid user challenge";
		return FALSE;
	}

	/* Read user response */
	len = rpa_read_buffer(request->pool, &p, end, &request->user_response);
	if (len != RPA_UCHALLENGE_LEN) {
		*error = "invalid user response";
		return FALSE;
	}

	if (p != end) {
		*error = "unneeded data found";
		return FALSE;
	}

	return TRUE;
}