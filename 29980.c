static int smbXcli_session_destructor(struct smbXcli_session *session)
{
	if (session->conn == NULL) {
		return 0;
	}

	DLIST_REMOVE(session->conn->sessions, session);
	return 0;
}
