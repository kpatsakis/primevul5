bool smb1cli_conn_server_writebraw(struct smbXcli_conn *conn)
{
	return conn->smb1.server.writebraw;
}
