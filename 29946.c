static int smbXcli_conn_destructor(struct smbXcli_conn *conn)
{
	/*
	 * NT_STATUS_OK, means we do not notify the callers
	 */
	smbXcli_conn_disconnect(conn, NT_STATUS_OK);

	while (conn->sessions) {
		conn->sessions->conn = NULL;
		DLIST_REMOVE(conn->sessions, conn->sessions);
	}

	if (conn->smb1.trans_enc) {
		TALLOC_FREE(conn->smb1.trans_enc);
	}

	return 0;
}
