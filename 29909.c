uint16_t smb1cli_req_mid(struct tevent_req *req)
{
	struct smbXcli_req_state *state =
		tevent_req_data(req,
		struct smbXcli_req_state);

	if (state->smb1.mid != 0) {
		return state->smb1.mid;
	}

	return SVAL(state->smb1.hdr, HDR_MID);
}
