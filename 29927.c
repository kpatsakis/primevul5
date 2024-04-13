bool smb2cli_conn_req_possible(struct smbXcli_conn *conn, uint32_t *max_dyn_len)
{
	uint16_t credits = 1;

	if (conn->smb2.cur_credits == 0) {
		if (max_dyn_len != NULL) {
			*max_dyn_len = 0;
		}
		return false;
	}

	if (conn->smb2.server.capabilities & SMB2_CAP_LARGE_MTU) {
		credits = conn->smb2.cur_credits;
	}

	if (max_dyn_len != NULL) {
		*max_dyn_len = credits * 65536;
	}

	return true;
}
