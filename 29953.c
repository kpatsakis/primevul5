static bool smbXcli_conn_receive_next(struct smbXcli_conn *conn)
{
	size_t num_pending = talloc_array_length(conn->pending);
	struct tevent_req *req;
	struct smbXcli_req_state *state;

	if (conn->read_smb_req != NULL) {
		return true;
	}

	if (num_pending == 0) {
		if (conn->smb2.mid < UINT64_MAX) {
			/* no more pending requests, so we are done for now */
			return true;
		}

		/*
		 * If there are no more SMB2 requests possible,
		 * because we are out of message ids,
		 * we need to disconnect.
		 */
		smbXcli_conn_disconnect(conn, NT_STATUS_CONNECTION_ABORTED);
		return true;
	}

	req = conn->pending[0];
	state = tevent_req_data(req, struct smbXcli_req_state);

	/*
	 * We're the first ones, add the read_smb request that waits for the
	 * answer from the server
	 */
	conn->read_smb_req = read_smb_send(conn->pending,
					   state->ev,
					   conn->sock_fd);
	if (conn->read_smb_req == NULL) {
		return false;
	}
	tevent_req_set_callback(conn->read_smb_req, smbXcli_conn_received, conn);
	return true;
}
