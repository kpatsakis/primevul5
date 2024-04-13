bool ntlmssp_check_request(const struct ntlmssp_request *request,
			   size_t data_size, const char **error)
{
	uint32_t flags;

	if (data_size < sizeof(struct ntlmssp_request)) {
		*error = "request too short";
		return FALSE;
	}

	if (read_le64(&request->magic) != NTLMSSP_MAGIC) {
		*error = "signature mismatch";
		return FALSE;
	}

	if (read_le32(&request->type) != NTLMSSP_MSG_TYPE1) {
		*error = "message type mismatch";
		return FALSE;
	}

	flags = read_le32(&request->flags);

	if ((flags & NTLMSSP_NEGOTIATE_NTLM) == 0) {
		*error = "client doesn't advertise NTLM support";
		return FALSE;
	}

	return TRUE;
}