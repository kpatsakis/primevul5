NTSTATUS smb1cli_session_set_session_key(struct smbXcli_session *session,
					 const DATA_BLOB _session_key)
{
	struct smbXcli_conn *conn = session->conn;
	uint8_t session_key[16];

	if (conn == NULL) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}

	if (session->smb1.application_key.length != 0) {
		/*
		 * TODO: do not allow this...
		 *
		 * return NT_STATUS_INVALID_PARAMETER_MIX;
		 */
		data_blob_clear_free(&session->smb1.application_key);
		session->smb1.protected_key = false;
	}

	if (_session_key.length == 0) {
		return NT_STATUS_OK;
	}

	ZERO_STRUCT(session_key);
	memcpy(session_key, _session_key.data,
	       MIN(_session_key.length, sizeof(session_key)));

	session->smb1.application_key = data_blob_talloc(session,
							 session_key,
							 sizeof(session_key));
	ZERO_STRUCT(session_key);
	if (session->smb1.application_key.data == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	session->smb1.protected_key = false;

	return NT_STATUS_OK;
}
