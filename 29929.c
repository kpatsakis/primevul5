uint16_t smb2cli_conn_server_security_mode(struct smbXcli_conn *conn)
{
	return conn->smb2.server.security_mode;
}
