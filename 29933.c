void smb2cli_conn_set_max_credits(struct smbXcli_conn *conn,
				  uint16_t max_credits)
{
	conn->smb2.max_credits = max_credits;
}
