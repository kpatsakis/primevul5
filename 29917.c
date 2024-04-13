NTSTATUS smb1cli_session_protect_session_key(struct smbXcli_session *session)
{
	if (session->smb1.protected_key) {
		/* already protected */
		return NT_STATUS_OK;
	}

	if (session->smb1.application_key.length != 16) {
		return NT_STATUS_INVALID_PARAMETER_MIX;
	}

	smb_key_derivation(session->smb1.application_key.data,
			   session->smb1.application_key.length,
			   session->smb1.application_key.data);

	session->smb1.protected_key = true;

	return NT_STATUS_OK;
}
