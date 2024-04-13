bool smb1cli_conn_server_lockread(struct smbXcli_conn *conn)
{
	return conn->smb1.server.lockread;
}
