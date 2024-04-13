static void smbXcli_conn_samba_suicide_done(struct tevent_req *subreq)
{
	struct tevent_req *req = tevent_req_callback_data(
		subreq, struct tevent_req);
	struct smbXcli_conn_samba_suicide_state *state = tevent_req_data(
		req, struct smbXcli_conn_samba_suicide_state);
	ssize_t nwritten;
	int err;

	state->write_req = NULL;

	nwritten = writev_recv(subreq, &err);
	TALLOC_FREE(subreq);
	if (nwritten == -1) {
		/* here, we need to notify all pending requests */
		NTSTATUS status = map_nt_error_from_unix_common(err);
		smbXcli_conn_disconnect(state->conn, status);
		return;
	}
	tevent_req_done(req);
}
