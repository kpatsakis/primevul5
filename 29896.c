uint32_t smb1cli_conn_server_session_key(struct smbXcli_conn *conn)
{
	return conn->smb1.server.session_key;
}
