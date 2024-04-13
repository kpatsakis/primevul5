void smbXcli_session_set_disconnect_expired(struct smbXcli_session *session)
{
	session->disconnect_expired = true;
}
