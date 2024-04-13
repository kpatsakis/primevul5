NTSTATUS smb2cli_req_get_sent_iov(struct tevent_req *req,
				  struct iovec *sent_iov)
{
	struct smbXcli_req_state *state =
		tevent_req_data(req,
		struct smbXcli_req_state);

	if (tevent_req_is_in_progress(req)) {
		return STATUS_PENDING;
	}

	sent_iov[0].iov_base = state->smb2.hdr;
	sent_iov[0].iov_len  = sizeof(state->smb2.hdr);

	sent_iov[1].iov_base = discard_const(state->smb2.fixed);
	sent_iov[1].iov_len  = state->smb2.fixed_len;

	if (state->smb2.dyn != NULL) {
		sent_iov[2].iov_base = discard_const(state->smb2.dyn);
		sent_iov[2].iov_len  = state->smb2.dyn_len;
	} else {
		sent_iov[2].iov_base = NULL;
		sent_iov[2].iov_len  = 0;
	}

	return NT_STATUS_OK;
}
