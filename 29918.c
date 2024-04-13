void smb1cli_session_set_id(struct smbXcli_session *session,
			    uint16_t session_id)
{
	session->smb1.session_id = session_id;
}
