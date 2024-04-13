bool smbXcli_session_is_authenticated(struct smbXcli_session *session)
{
	const DATA_BLOB *application_key;

	if (session->conn == NULL) {
		return false;
	}

	/*
	 * If we have an application key we had a session key negotiated
	 * at auth time.
	 */
	if (session->conn->protocol >= PROTOCOL_SMB2_02) {
		application_key = &session->smb2->application_key;
	} else {
		application_key = &session->smb1.application_key;
	}

	if (application_key->length == 0) {
		return false;
	}

	return true;
}
