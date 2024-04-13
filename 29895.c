uint16_t smb1cli_conn_server_security_mode(struct smbXcli_conn *conn)
{
	return conn->smb1.server.security_mode;
}
