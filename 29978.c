struct smbXcli_session *smbXcli_session_copy(TALLOC_CTX *mem_ctx,
						struct smbXcli_session *src)
{
	struct smbXcli_session *session;

	session = talloc_zero(mem_ctx, struct smbXcli_session);
	if (session == NULL) {
		return NULL;
	}
	session->smb2 = talloc_zero(session, struct smb2cli_session);
	if (session->smb2 == NULL) {
		talloc_free(session);
		return NULL;
	}

	session->conn = src->conn;
	*session->smb2 = *src->smb2;
	session->smb2_channel = src->smb2_channel;
	session->disconnect_expired = src->disconnect_expired;

	DLIST_ADD_END(src->conn->sessions, session, struct smbXcli_session *);
	talloc_set_destructor(session, smbXcli_session_destructor);

	return session;
}
