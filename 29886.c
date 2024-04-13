uint32_t smb1cli_conn_capabilities(struct smbXcli_conn *conn)
{
	return conn->smb1.capabilities;
}
