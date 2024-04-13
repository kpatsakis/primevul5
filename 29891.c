bool smb1cli_conn_req_possible(struct smbXcli_conn *conn)
{
	size_t pending = talloc_array_length(conn->pending);
	uint16_t possible = conn->smb1.server.max_mux;

	if (pending >= possible) {
		return false;
	}

	return true;
}
