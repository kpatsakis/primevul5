bool ntlmssp_check_response(const struct ntlmssp_response *response,
			    size_t data_size, const char **error)
{
	if (data_size < sizeof(struct ntlmssp_response)) {
		*error = "response too short";
		return FALSE;
	}

	if (read_le64(&response->magic) != NTLMSSP_MAGIC) {
		*error = "signature mismatch";
		return FALSE;
	}

	if (read_le32(&response->type) != NTLMSSP_MSG_TYPE3) {
		*error = "message type mismatch";
		return FALSE;
	}

	if (!ntlmssp_check_buffer(&response->lm_response, data_size, error) ||
	    !ntlmssp_check_buffer(&response->ntlm_response, data_size, error) ||
	    !ntlmssp_check_buffer(&response->domain, data_size, error) ||
	    !ntlmssp_check_buffer(&response->user, data_size, error) ||
	    !ntlmssp_check_buffer(&response->workstation, data_size, error))
		return FALSE;

	return TRUE;
}