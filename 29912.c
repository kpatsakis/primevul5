uint32_t smb1cli_req_seqnum(struct tevent_req *req)
{
	struct smbXcli_req_state *state =
		tevent_req_data(req,
		struct smbXcli_req_state);

	return state->smb1.seqnum;
}
