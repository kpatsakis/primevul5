struct smbXcli_session *smbXcli_session_create(TALLOC_CTX *mem_ctx,
					       struct smbXcli_conn *conn)
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
	talloc_set_destructor(session, smbXcli_session_destructor);

	DLIST_ADD_END(conn->sessions, session, struct smbXcli_session *);
	session->conn = conn;

	memcpy(session->smb2_channel.preauth_sha512,
	       conn->smb2.preauth_sha512,
	       sizeof(session->smb2_channel.preauth_sha512));

	return session;
}
