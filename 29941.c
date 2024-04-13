void smb2cli_req_set_notify_async(struct tevent_req *req)
{
	struct smbXcli_req_state *state =
		tevent_req_data(req,
		struct smbXcli_req_state);

	state->smb2.notify_async = true;
}
