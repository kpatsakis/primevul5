const DATA_BLOB *smbXcli_conn_server_gss_blob(struct smbXcli_conn *conn)
{
	if (conn->protocol >= PROTOCOL_SMB2_02) {
		return &conn->smb2.server.gss_blob;
	}

	return &conn->smb1.server.gss_blob;
}
