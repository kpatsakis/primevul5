int smb1cli_conn_server_time_zone(struct smbXcli_conn *conn)
{
	return conn->smb1.server.time_zone;
}
