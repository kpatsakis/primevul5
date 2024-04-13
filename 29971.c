static struct tevent_req *smbXcli_negprot_smb1_subreq(struct smbXcli_negprot_state *state)
{
	size_t i;
	DATA_BLOB bytes = data_blob_null;
	uint8_t flags;
	uint16_t flags2;

	/* setup the protocol strings */
	for (i=0; i < ARRAY_SIZE(smb1cli_prots); i++) {
		uint8_t c = 2;
		bool ok;

		if (smb1cli_prots[i].proto < state->conn->min_protocol) {
			continue;
		}

		if (smb1cli_prots[i].proto > state->conn->max_protocol) {
			continue;
		}

		ok = data_blob_append(state, &bytes, &c, sizeof(c));
		if (!ok) {
			return NULL;
		}

		/*
		 * We now it is already ascii and
		 * we want NULL termination.
		 */
		ok = data_blob_append(state, &bytes,
				      smb1cli_prots[i].smb1_name,
				      strlen(smb1cli_prots[i].smb1_name)+1);
		if (!ok) {
			return NULL;
		}
	}

	smb1cli_req_flags(state->conn->max_protocol,
			  state->conn->smb1.client.capabilities,
			  SMBnegprot,
			  0, 0, &flags,
			  0, 0, &flags2);

	return smb1cli_req_send(state, state->ev, state->conn,
				SMBnegprot,
				flags, ~flags,
				flags2, ~flags2,
				state->timeout_msec,
				0xFFFE, 0, NULL, /* pid, tid, session */
				0, NULL, /* wct, vwv */
				bytes.length, bytes.data);
}
