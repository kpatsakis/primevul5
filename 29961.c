const struct GUID *smbXcli_conn_server_guid(struct smbXcli_conn *conn)
{
	if (conn->protocol >= PROTOCOL_SMB2_02) {
		return &conn->smb2.server.guid;
	}

	return &conn->smb1.server.guid;
}
