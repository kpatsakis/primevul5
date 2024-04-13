NTTIME smbXcli_conn_server_system_time(struct smbXcli_conn *conn)
{
	if (conn->protocol >= PROTOCOL_SMB2_02) {
		return conn->smb2.server.system_time;
	}

	return conn->smb1.server.system_time;
}
