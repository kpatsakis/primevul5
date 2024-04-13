uint32_t smb2cli_conn_max_write_size(struct smbXcli_conn *conn)
{
	return conn->smb2.server.max_write_size;
}
