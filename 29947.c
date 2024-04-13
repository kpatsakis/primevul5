bool smbXcli_conn_dfs_supported(struct smbXcli_conn *conn)
{
	if (conn->protocol >= PROTOCOL_SMB2_02) {
		return (smb2cli_conn_server_capabilities(conn) & SMB2_CAP_DFS);
	}

	return (smb1cli_conn_capabilities(conn) & CAP_DFS);
}
