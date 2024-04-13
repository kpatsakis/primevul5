bool smb1cli_conn_encryption_on(struct smbXcli_conn *conn)
{
	return common_encryption_on(conn->smb1.trans_enc);
}
