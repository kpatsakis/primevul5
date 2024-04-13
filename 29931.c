void smb2cli_conn_set_cc_max_chunks(struct smbXcli_conn *conn,
				    uint32_t max_chunks)
{
	conn->smb2.cc_max_chunks = max_chunks;
}
