bool smb1cli_conn_activate_signing(struct smbXcli_conn *conn,
				   const DATA_BLOB user_session_key,
				   const DATA_BLOB response)
{
	return smb_signing_activate(conn->smb1.signing,
				    user_session_key,
				    response);
}
