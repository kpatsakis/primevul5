uint32_t smb2cli_conn_cc_chunk_len(struct smbXcli_conn *conn)
{
	return conn->smb2.cc_chunk_len;
}
