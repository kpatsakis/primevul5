bool smbXcli_req_set_pending(struct tevent_req *req)
{
	struct smbXcli_req_state *state =
		tevent_req_data(req,
		struct smbXcli_req_state);
	struct smbXcli_conn *conn;
	struct tevent_req **pending;
	size_t num_pending;

	conn = state->conn;

	if (!smbXcli_conn_is_connected(conn)) {
		return false;
	}

	num_pending = talloc_array_length(conn->pending);

	pending = talloc_realloc(conn, conn->pending, struct tevent_req *,
				 num_pending+1);
	if (pending == NULL) {
		return false;
	}
	pending[num_pending] = req;
	conn->pending = pending;
	tevent_req_set_cleanup_fn(req, smbXcli_req_cleanup);
	tevent_req_set_cancel_fn(req, smbXcli_req_cancel);

	if (!smbXcli_conn_receive_next(conn)) {
		/*
		 * the caller should notify the current request
		 *
		 * And all other pending requests get notified
		 * by smbXcli_conn_disconnect().
		 */
		smbXcli_req_unset_pending(req);
		smbXcli_conn_disconnect(conn, NT_STATUS_NO_MEMORY);
		return false;
	}

	return true;
}
