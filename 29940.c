void smb2cli_req_set_credit_charge(struct tevent_req *req, uint16_t charge)
{
	struct smbXcli_req_state *state =
		tevent_req_data(req,
		struct smbXcli_req_state);

	state->smb2.credit_charge = charge;
}
