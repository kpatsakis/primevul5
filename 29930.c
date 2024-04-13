void smb2cli_conn_set_cc_chunk_len(struct smbXcli_conn *conn,
				    uint32_t chunk_len)
{
	conn->smb2.cc_chunk_len = chunk_len;
}
