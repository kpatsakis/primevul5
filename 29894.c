bool smb1cli_conn_server_readbraw(struct smbXcli_conn *conn)
{
	return conn->smb1.server.readbraw;
}
