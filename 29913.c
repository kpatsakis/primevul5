size_t smb1cli_req_wct_ofs(struct tevent_req **reqs, int num_reqs)
{
	size_t wct_ofs;
	int i;

	wct_ofs = HDR_WCT;

	for (i=0; i<num_reqs; i++) {
		struct smbXcli_req_state *state;
		state = tevent_req_data(reqs[i], struct smbXcli_req_state);
		wct_ofs += smbXcli_iov_len(state->smb1.iov+2,
					   state->smb1.iov_count-2);
		wct_ofs = (wct_ofs + 3) & ~3;
	}
	return wct_ofs;
}
