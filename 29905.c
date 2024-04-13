static bool smb1cli_req_cancel(struct tevent_req *req)
{
	struct smbXcli_req_state *state =
		tevent_req_data(req,
		struct smbXcli_req_state);
	uint8_t flags;
	uint16_t flags2;
	uint32_t pid;
	uint16_t mid;
	struct tevent_req *subreq;
	NTSTATUS status;

	flags = CVAL(state->smb1.hdr, HDR_FLG);
	flags2 = SVAL(state->smb1.hdr, HDR_FLG2);
	pid  = SVAL(state->smb1.hdr, HDR_PID);
	pid |= SVAL(state->smb1.hdr, HDR_PIDHIGH)<<16;
	mid = SVAL(state->smb1.hdr, HDR_MID);

	subreq = smb1cli_req_create(state, state->ev,
				    state->conn,
				    SMBntcancel,
				    flags, 0,
				    flags2, 0,
				    0, /* timeout */
				    pid,
				    state->tcon,
				    state->session,
				    0, NULL, /* vwv */
				    0, NULL); /* bytes */
	if (subreq == NULL) {
		return false;
	}
	smb1cli_req_set_mid(subreq, mid);

	status = smb1cli_req_chain_submit(&subreq, 1);
	if (!NT_STATUS_IS_OK(status)) {
		TALLOC_FREE(subreq);
		return false;
	}
	smb1cli_req_set_mid(subreq, 0);

	tevent_req_set_callback(subreq, smb1cli_req_cancel_done, NULL);

	return true;
}
