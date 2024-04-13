uint16_t smbXcli_conn_max_requests(struct smbXcli_conn *conn)
{
	if (conn->protocol >= PROTOCOL_SMB2_02) {
		/*
		 * TODO...
		 */
		return 1;
	}

	return conn->smb1.server.max_mux;
}
