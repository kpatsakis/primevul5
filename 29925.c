uint32_t smb2cli_conn_max_trans_size(struct smbXcli_conn *conn)
{
	return conn->smb2.server.max_trans_size;
}
