bool smb1cli_conn_server_writeunlock(struct smbXcli_conn *conn)
{
	return conn->smb1.server.writeunlock;
}
