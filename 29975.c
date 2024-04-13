static void smbXcli_req_cleanup(struct tevent_req *req,
				enum tevent_req_state req_state)
{
	struct smbXcli_req_state *state =
		tevent_req_data(req,
		struct smbXcli_req_state);

	TALLOC_FREE(state->write_req);

	switch (req_state) {
	case TEVENT_REQ_RECEIVED:
		/*
		 * Make sure we really remove it from
		 * the pending array on destruction.
		 */
		state->smb1.mid = 0;
		smbXcli_req_unset_pending(req);
		return;
	default:
		return;
	}
}
