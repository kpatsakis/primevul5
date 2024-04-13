struct tevent_req *smb2cli_validate_negotiate_info_send(TALLOC_CTX *mem_ctx,
						struct tevent_context *ev,
						struct smbXcli_conn *conn,
						uint32_t timeout_msec,
						struct smbXcli_session *session,
						struct smbXcli_tcon *tcon)
{
	struct tevent_req *req;
	struct smb2cli_validate_negotiate_info_state *state;
	uint8_t *buf;
	uint16_t dialect_count = 0;
	struct tevent_req *subreq;
	bool _save_should_sign;
	size_t i;

	req = tevent_req_create(mem_ctx, &state,
				struct smb2cli_validate_negotiate_info_state);
	if (req == NULL) {
		return NULL;
	}
	state->conn = conn;

	state->in_input_buffer = data_blob_talloc_zero(state,
					4 + 16 + 1 + 1 + 2);
	if (tevent_req_nomem(state->in_input_buffer.data, req)) {
		return tevent_req_post(req, ev);
	}
	buf = state->in_input_buffer.data;

	if (state->conn->max_protocol >= PROTOCOL_SMB2_22) {
		SIVAL(buf, 0, conn->smb2.client.capabilities);
	} else {
		SIVAL(buf, 0, 0); /* Capabilities */
	}
	if (state->conn->max_protocol >= PROTOCOL_SMB2_10) {
		NTSTATUS status;
		DATA_BLOB blob;

		status = GUID_to_ndr_blob(&conn->smb2.client.guid,
					  state, &blob);
		if (!NT_STATUS_IS_OK(status)) {
			return NULL;
		}
		memcpy(buf+4, blob.data, 16); /* ClientGuid */
	} else {
		memset(buf+4, 0, 16);	/* ClientGuid */
	}
	if (state->conn->min_protocol >= PROTOCOL_SMB2_02) {
		SCVAL(buf, 20, conn->smb2.client.security_mode);
	} else {
		SCVAL(buf, 20, 0);
	}
	SCVAL(buf, 21, 0); /* reserved */

	for (i=0; i < ARRAY_SIZE(smb2cli_prots); i++) {
		bool ok;
		size_t ofs;

		if (smb2cli_prots[i].proto < state->conn->min_protocol) {
			continue;
		}

		if (smb2cli_prots[i].proto > state->conn->max_protocol) {
			continue;
		}

		if (smb2cli_prots[i].proto == state->conn->protocol) {
			state->dialect = smb2cli_prots[i].smb2_dialect;
		}

		ofs = state->in_input_buffer.length;
		ok = data_blob_realloc(state, &state->in_input_buffer,
				       ofs + 2);
		if (!ok) {
			tevent_req_oom(req);
			return tevent_req_post(req, ev);
		}

		buf = state->in_input_buffer.data;
		SSVAL(buf, ofs, smb2cli_prots[i].smb2_dialect);

		dialect_count++;
	}
	buf = state->in_input_buffer.data;
	SSVAL(buf, 22, dialect_count);

	_save_should_sign = smb2cli_tcon_is_signing_on(tcon);
	smb2cli_tcon_should_sign(tcon, true);
	subreq = smb2cli_ioctl_send(state, ev, conn,
				    timeout_msec, session, tcon,
				    UINT64_MAX, /* in_fid_persistent */
				    UINT64_MAX, /* in_fid_volatile */
				    FSCTL_VALIDATE_NEGOTIATE_INFO,
				    0, /* in_max_input_length */
				    &state->in_input_buffer,
				    24, /* in_max_output_length */
				    &state->in_output_buffer,
				    SMB2_IOCTL_FLAG_IS_FSCTL);
	smb2cli_tcon_should_sign(tcon, _save_should_sign);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq,
				smb2cli_validate_negotiate_info_done,
				req);

	return req;
}
