uint8_t smb2cli_conn_get_io_priority(struct smbXcli_conn *conn)
{
	if (conn->protocol < PROTOCOL_SMB3_11) {
		return 0;
	}

	return conn->smb2.io_priority;
}
