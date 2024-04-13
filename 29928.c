uint32_t smb2cli_conn_server_capabilities(struct smbXcli_conn *conn)
{
	return conn->smb2.server.capabilities;
}
