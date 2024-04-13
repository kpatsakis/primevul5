uint32_t smb2cli_conn_cc_max_chunks(struct smbXcli_conn *conn)
{
	return conn->smb2.cc_max_chunks;
}
