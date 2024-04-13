bool smbXcli_conn_is_connected(struct smbXcli_conn *conn)
{
	if (conn == NULL) {
		return false;
	}

	if (conn->sock_fd == -1) {
		return false;
	}

	return true;
}
