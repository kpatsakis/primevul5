static bool cli_dfs_check_error(struct cli_state *cli, NTSTATUS expected,
				NTSTATUS status)
{
	/* only deal with DS when we negotiated NT_STATUS codes and UNICODE */

	if (!(smbXcli_conn_use_unicode(cli->conn))) {
		return false;
	}
	if (!(smb1cli_conn_capabilities(cli->conn) & CAP_STATUS32)) {
		return false;
	}
	if (NT_STATUS_EQUAL(status, expected)) {
		return true;
	}
	return false;
}
