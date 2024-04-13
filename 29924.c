uint32_t smb2cli_conn_max_read_size(struct smbXcli_conn *conn)
{
	return conn->smb2.server.max_read_size;
}
