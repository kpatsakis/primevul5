bool smb1cli_conn_signing_is_active(struct smbXcli_conn *conn)
{
	return smb_signing_is_active(conn->smb1.signing);
}
