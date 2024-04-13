const uint8_t *smb1cli_conn_server_challenge(struct smbXcli_conn *conn)
{
	return conn->smb1.server.challenge;
}
